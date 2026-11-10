#!/usr/bin/env bash
set -u

usage() {
    cat <<'EOF'
Run the milliSoC FPGA hardware regression sequentially.

Usage:
  bash tools/run_fpga_regression.sh [--include-reset-control] [app ...]
  bash tools/run_fpga_regression.sh --list

With no app names, runs all regular app tests. reset_control is excluded by
default because it requires boot-ROM behavior not present in the current FPGA
image. Pass --include-reset-control to run it anyway. Supplying app names runs
only those apps; supported app names are shown by --list.

The script forwards make's usual hardware variables, including M85_COM,
R5_COM, UART_BAUD, DSTREAM_CONN, CDB_ROOT, CDB_ENTRY, HW_PYTHON,
BOOT_TIMEOUT, and FLASH_TIMEOUT.

u85_model is built with APPEND_MODEL=1 so its model is programmed with the app.
Logs are saved under build/<app>/ by tools/hw_run.py.
EOF
}

supported_apps=(
    hello
    bus_access
    debug_access
    fpu_tests
    mhu_tests
    sram_tests
    timer_tests
    u85_tests
    u85_control
    u85_model
    reset_control
)

default_apps=(
    hello
    bus_access
    debug_access
    fpu_tests
    mhu_tests
    sram_tests
    timer_tests
    u85_tests
    u85_control
    u85_model
)

include_reset_control=0
list_apps=0
requested_apps=()

for arg in "$@"; do
    case "$arg" in
        --help|-h)
            usage
            exit 0
            ;;
        --include-reset-control)
            include_reset_control=1
            ;;
        --list)
            list_apps=1
            ;;
        -*)
            printf 'Unknown option: %s\n\n' "$arg" >&2
            usage >&2
            exit 2
            ;;
        *)
            requested_apps+=("$arg")
            ;;
    esac
done

if (( list_apps )); then
    printf '%s\n' "${supported_apps[@]}"
    exit 0
fi

if ((${#requested_apps[@]})); then
    apps=("${requested_apps[@]}")
elif (( include_reset_control )); then
    apps=("${default_apps[@]}" reset_control)
else
    apps=("${default_apps[@]}")
fi

for app in "${apps[@]}"; do
    found=0
    for supported in "${supported_apps[@]}"; do
        if [[ "$app" == "$supported" ]]; then
            found=1
            break
        fi
    done
    if (( ! found )); then
        printf 'Unsupported app: %s\nRun with --list to see supported apps.\n' "$app" >&2
        exit 2
    fi
done

if (( include_reset_control )) && ((${#requested_apps[@]})) ; then
    found=0
    for app in "${apps[@]}"; do
        if [[ "$app" == reset_control ]]; then
            found=1
            break
        fi
    done
    if (( ! found )); then
        apps+=(reset_control)
    fi
fi

reset_control_included=0
for app in "${apps[@]}"; do
    if [[ "$app" == reset_control ]]; then
        reset_control_included=1
        break
    fi
done

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
if ! command -v make >/dev/null 2>&1; then
    echo "Error: make is not on PATH. Run this script from Git Bash / Arm DS shell." >&2
    exit 2
fi

results=()
failed=0

printf 'Running FPGA regression (%d app(s))\n' "${#apps[@]}"
printf 'Repository: %s\n' "$repo_root"
printf 'Logs:       %s/build/<app>/\n\n' "$repo_root"

for app in "${apps[@]}"; do
    append_model=0
    if [[ "$app" == u85_model ]]; then
        append_model=1
    fi

    printf '\n========== %s ==========\n' "$app"
    if make -C "$repo_root" run_hw TESTNAME="$app" APPEND_MODEL="$append_model"; then
        results+=("PASS  $app")
    else
        status=$?
        printf 'Regression step failed for %s (make exit %d)\n' "$app" "$status" >&2
        results+=("FAIL  $app (exit $status)")
        failed=1
    fi
done

if (( ! reset_control_included )); then
    printf '\nSKIP  reset_control (known incompatible with the current FPGA boot ROM; use --include-reset-control to try it)\n'
fi

printf '\n========== FPGA regression summary ==========\n'
for result in "${results[@]}"; do
    printf '%s\n' "$result"
done
if (( ! reset_control_included )); then
    printf '%s\n' 'SKIP  reset_control'
fi

if (( failed )); then
    exit 1
fi
exit 0
