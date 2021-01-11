#include <smolrtsp/request.h>

#include <string.h>

#include "nala.h"

static void check(Slice99 request, SmolRTSP_Request expected) {
    SmolRTSP_Request result = {
        .header_map =
            {
                .headers = (SmolRTSP_Header[3]){0},
                .len = 0,
                .size = 3,
            },
    };
    SmolRTSP_RequestDeserializerState state = SMOLRTSP_REQUEST_DESERIALIZER_START_STATE;
    const SmolRTSP_DeserializeResult res = SmolRTSP_Request_deserialize(&result, &request, &state);

    ASSERT_EQ(res, SmolRTSP_DeserializeResultOk);
    ASSERT_EQ(state.tag, SmolRTSP_RequestDeserializerStateDone);
    ASSERT(SmolRTSP_Request_eq(result, expected));
}

TEST(deserialize_request) {
    SmolRTSP_Header headers[] = {
        {
            SMOLRTSP_HEADER_NAME_CONTENT_LENGTH,
            Slice99_from_str("10"),
        },
        {
            SMOLRTSP_HEADER_NAME_ACCEPT_LANGUAGE,
            Slice99_from_str("English"),
        },
        {
            SMOLRTSP_HEADER_NAME_CONTENT_TYPE,
            Slice99_from_str("application/octet-stream"),
        },
    };

    const SmolRTSP_Request expected = {
        .start_line =
            {
                .method = SMOLRTSP_METHOD_DESCRIBE,
                .uri = Slice99_from_str("http://example.com"),
                .version = {.major = 1, .minor = 1},
            },
        .header_map =
            {
                .len = 3,
                .size = 3,
                .headers = headers,
            },
        .body = Slice99_from_str("0123456789"),
    };

    check(
        Slice99_from_str("DESCRIBE http://example.com RTSP/1.1\r\n"
                         "Content-Length: 10\r\nAccept-Language: English\r\nContent-Type: "
                         "application/octet-stream\r\n\r\n"
                         "0123456789"),
        expected);
}
