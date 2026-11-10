# SST26VF064B programmer for Arm Development Studio (Jython 2.7)
# Input: plain hex text, exactly one byte per line.
# Run: source sst26_programmer.py "C:/path/image.hex" 0x000000
#
# Uses standard SPI and the SoC Labs AHB-QSPI controller at 0x44010000.
# Controller address window is 4 MiB (22-bit), although SST26VF064B is 8 MiB.
#
# WARNING: UNLOCK_ALL_BLOCKS=True sends WREN + ULBPR (98h), clearing all
# non-permanently-locked block-protection bits. The script erases every 4-KiB
# sector touched by the image. flash_base must be 4-KiB aligned.

import sys, re, time
from arm_ds.debugger_v1 import Debugger

CTRL = 0x44010000
CSTAT = 0x44010004
CMD = 0x44010008
ADDR = 0x4401000C
RDATA = 0x44010010
WDATA = 0x44010020

FLASH_LIMIT = 0x00400000
SECTOR = 4096
PAGE = 256
MAX_XFER = 16
UNLOCK_ALL_BLOCKS = True
VERIFY_ERASE = True

def log(s):
    print("[SST26] " + str(s))

def ds(s):
    return str(ec.executeDSCommand(s))

def wr(a, v):
    ds("memory set AXI:0x%08X 32 0x%08X" % (a, v & 0xFFFFFFFF))

def wr4(a, v0, v1, v2, v3):
    """Write four consecutive 32-bit words in one debugger command."""
    ds("memory set AXI:0x%08X 32 {0x%08X,0x%08X,0x%08X,0x%08X}" %
       (a, v0 & 0xFFFFFFFF, v1 & 0xFFFFFFFF,
        v2 & 0xFFFFFFFF, v3 & 0xFFFFFFFF))

def rd(a, n):
    out = ds("x/%dwx AXI:0x%08X" % (n, a))
    vals = re.findall(r"0x([0-9A-Fa-f]+)", out)
    if len(vals) < n:
        raise Exception("Cannot parse debugger read: " + out)
    return [int(x, 16) & 0xFFFFFFFF for x in vals[-n:]]

def wait_controller_idle(timeout=2.0):
    t0 = time.time()
    while True:
        v = rd(CSTAT, 1)[0]
        if (v & 1) == 0:
            return
        if time.time() - t0 > timeout:
            raise Exception("QSPI controller busy timeout, STATUS=0x%08X" % v)
        time.sleep(0.001)

def command(v):
    wr(CMD, v)
    # SPI_CMD.ENABLE (bit 8) clears when controller acknowledges command.
    t0 = time.time()
    while rd(CMD, 1)[0] & 0x100:
        if time.time() - t0 > 2.0:
            raise Exception("SPI_CMD acknowledgement timeout: 0x%08X" % v)
        time.sleep(0.001)
    wait_controller_idle()

def status():
    command(0x00000305)             # SST26 RDSR 05h, one byte
    return (rd(RDATA, 1)[0] >> 24) & 0xFF

def force_flash_spi_mode():
    log("Forcing SST26 into standard SPI mode")

    # First attempt RSTQIO while the controller is configured for SQI.
    wr(CTRL, 0x00000001)
    wait_controller_idle()
    command(0x000001FF)

    # Switch the controller to standard SPI.
    wr(CTRL, 0x00000000)
    wait_controller_idle()

    # Issue RSTQIO again in standard SPI mode.
    command(0x000001FF)

    log("Flash and controller configured for standard SPI")

def wait_ready(timeout, what):
    t0 = time.time()
    while True:
        s = status()
        # SST26 BUSY is represented in status bits 0 and 7.
        if (s & 0x81) == 0:
            return s
        if time.time() - t0 > timeout:
            raise Exception("%s timeout, SST26 STATUS=0x%02X" % (what, s))
        time.sleep(0.01)

def wren():
    command(0x00000106)             # WREN 06h
    s = status()
    if (s & 0x02) == 0:
        raise Exception("WEL did not set after WREN, STATUS=0x%02X" % s)

def jedec_id():
    command(0x0002039F)             # JEDEC ID 9Fh, three bytes
    return rd(RDATA, 1)[0]

def unlock_all():
    log("WREN + Global Block Protection Unlock (ULBPR 98h)")
    wren()
    command(0x00000198)
    log("STATUS after ULBPR: 0x%02X" % status())

def read_data(address, count):
    if count < 1 or count > MAX_XFER:
        raise Exception("Invalid read count %d" % count)
    wr(ADDR, address)
    # READ 03h, READ + ADDR_EN, N_RW_BYTES=count-1 in bits [19:16].
    command((((count - 1) & 0xF) << 16) | 0x00000B03)
    words = rd(RDATA, (count + 3) // 4)
    result = []
    for w in words:
        result.extend([w & 255, (w >> 8) & 255, (w >> 16) & 255, (w >> 24) & 255])
    return result[:count]

def erase_sector(address):
    wren()
    wr(ADDR, address)
    command(0x00000920)             # Sector Erase 20h + ADDR_EN
    wait_ready(5.0, "sector erase 0x%06X" % address)

def program(address, data):
    n = len(data)
    if n < 1 or n > MAX_XFER:
        raise Exception("Invalid program length %d" % n)
    if (address % PAGE) + n > PAGE:
        raise Exception("Program transaction crosses 256-byte page boundary")
    wren()
    wr(ADDR, address)
    padded = list(data) + [0xFF] * (MAX_XFER - n)
    words = []
    for k in range(4):
        b = padded[k*4:k*4+4]
        words.append(b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24))
    wr4(WDATA, words[0], words[1], words[2], words[3])
    # Page Program 02h, WRITE + ADDR_EN, count-1 in [19:16].
    command((((n - 1) & 0xF) << 16) | 0x00000D02)
    wait_ready(5.0, "page program 0x%06X" % address)

def parse_file(filename):
    data = []
    f = open(filename, "r")
    try:
        line_no = 0
        for raw in f:
            line_no += 1
            token = raw.strip()
            if not token or token.startswith("#") or token.startswith("//"):
                continue
            if token.lower().startswith("0x"):
                token = token[2:]
            if len(token) != 2:
                raise Exception("Line %d must contain exactly one byte (2 hex digits): '%s'" %
                                (line_no, token))
            try:
                data.append(int(token, 16))
            except:
                raise Exception("Invalid hex byte on line %d: '%s'" % (line_no, token))
    finally:
        f.close()
    if not data:
        raise Exception("Input file is empty")
    return data

def chunks_for(data, base):
    result = []
    off = 0
    while off < len(data):
        a = base + off
        n = min(MAX_XFER, PAGE - (a % PAGE), len(data) - off)
        result.append((a, data[off:off+n]))
        off += n
    return result

def verify_erased(address):
    got = read_data(address, 16)
    if got != [255] * 16:
        raise Exception("Erase verify failed at 0x%06X: %s" %
                        (address, " ".join(["%02X" % x for x in got])))

def main():
    if len(sys.argv) < 3:
        raise Exception('Usage: source sst26_programmer.py "image.hex" <flash_base>')
    filename = sys.argv[1]
    base = int(sys.argv[2], 0)
    if base < 0 or base >= FLASH_LIMIT:
        raise Exception("Base address outside 4-MiB controller window")
    if base % SECTOR:
        raise Exception("flash_base must be 4-KiB aligned")

    data = parse_file(filename)
    end = base + len(data)
    if end > FLASH_LIMIT:
        raise Exception("Image exceeds controller 4-MiB address window")
    first_sector = base
    last_sector = ((end - 1) // SECTOR) * SECTOR
    nchunks = chunks_for(data, base)

    log("Image: %d bytes" % len(data))
    log("Destination: 0x%06X..0x%06X" % (base, end - 1))
    log("Sectors to erase: %d" % (((last_sector-first_sector)//SECTOR)+1))
    log("Program transactions: %d (max 16 bytes)" % len(nchunks))

    force_flash_spi_mode()        # SPI mode, APB command interface
    wait_controller_idle()
    jid = jedec_id()
    log("JEDEC ID raw RDATA0: 0x%08X" % jid)
    if jid in (0, 0xFFFFFFFF):
        raise Exception("Invalid JEDEC ID; aborting")
    wait_ready(5.0, "initial ready check")

    if UNLOCK_ALL_BLOCKS:
        unlock_all()
    else:
        log("Skipping global block protection unlock")

    t_phase = time.time()
    log("Erasing...")
    a = first_sector
    while a <= last_sector:
        log("  4-KiB sector 0x%06X" % a)
        erase_sector(a)
        if VERIFY_ERASE:
            verify_erased(a)
        a += SECTOR
    log("Erase phase: %.1f s" % (time.time() - t_phase))

    t_phase = time.time()
    log("Programming...")
    for i, (a, payload) in enumerate(nchunks):
        program(a, payload)
        if (i + 1) % 64 == 0 or i + 1 == len(nchunks):
            log("  %d/%d transactions" % (i + 1, len(nchunks)))
    log("Program phase: %.1f s" % (time.time() - t_phase))

    t_phase = time.time()
    log("Verifying...")
    done = 0
    for a, payload in nchunks:
        actual = read_data(a, len(payload))
        for i, expected in enumerate(payload):
            if actual[i] != expected:
                raise Exception("VERIFY FAIL at 0x%06X: expected %02X, got %02X" %
                                (a+i, expected, actual[i]))
        done += len(payload)
    log("Verify phase: %.1f s" % (time.time() - t_phase))

    log("SUCCESS: %d bytes programmed and verified." % done)
    log("Range: 0x%06X..0x%06X" % (base, end - 1))

debugger = Debugger()
ec = debugger.getCurrentExecutionContext()
try:
    main()
except Exception, e:
    log("ERROR: %s" % e)
    raise

