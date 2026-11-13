/*
 * Test vectors for hello_world_int8.
 *
 * Hand-written: this model's vector set predates tools/gen_test_vectors.py and
 * is the quantised sine sweep from the TensorFlow Lite Micro hello_world
 * example.  The layout matches the generated headers so the runtime does not
 * care which model it is running (see models/vectors/micro_speech_int8.h for
 * the generated form).
 *
 * Model   : hello_world_int8.tflite (2704 bytes)
 * SHA-256 : 505ee4fae7fa46ab67bea4c08b4969eb3eb8b9114c50595ec4a29d9a27993202
 * Input   : [1] int8 (1 byte)
 * Output  : [1] int8 (1 byte)
 *
 * The expected outputs come from the TFLite reference kernels, so the NPU
 * result is allowed MODEL_TEST_TOLERANCE LSB of difference.
 */

#ifndef MODEL_VECTORS_H
#define MODEL_VECTORS_H

#define MODEL_TEST_NAME          "hello_world_int8"
#define MODEL_TEST_INPUT_BYTES   1
#define MODEL_TEST_OUTPUT_BYTES  1
#define MODEL_TEST_TOLERANCE     2
#define MODEL_TEST_COUNT         9

static const int8_t model_test_inputs[9][1] = {
    { -128 },
    {  -96 },
    {  -64 },
    {  -32 },
    {    0 },
    {   32 },
    {   64 },
    {   96 },
    {  127 },
};

static const int8_t model_test_expected[9][1] = {
    {    4 },
    {   89 },
    {  126 },
    {   90 },
    {    4 },
    {  -74 },
    { -126 },
    {  -88 },
    {   -9 },
};

#endif /* MODEL_VECTORS_H */
