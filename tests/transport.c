#include <smolrtsp/transport.h>

#include <greatest.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>

#define DATA_0 "abc"
#define DATA_1 "defghi"

static struct iovec bufs[] = {
    {.iov_base = DATA_0, .iov_len = sizeof((char[]){DATA_0}) - 1},
    {.iov_base = DATA_1, .iov_len = sizeof((char[]){DATA_1}) - 1},
};

static enum greatest_test_res test_transport(
    SmolRTSP_Transport t, int read_fd, size_t expected_len,
    const char expected[restrict static expected_len]) {
    const bool transmit_ok =
        VCALL(t, transmit, (SmolRTSP_IoVecSlice)Slice99_typed_from_array(bufs)) == 0;
    ASSERT(transmit_ok);

    char *buffer = malloc(expected_len);
    ASSERT(buffer);
    const bool read_ok = read(read_fd, buffer, expected_len) == (ssize_t)expected_len;
    ASSERT(read_ok);

    ASSERT_MEM_EQ(expected, buffer, expected_len);

    free(buffer);
    VCALL_SUPER(t, SmolRTSP_Droppable, drop);

    PASS();
}

TEST check_tcp(void) {
    int fds[2];
    const bool socketpair_ok = socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0;
    ASSERT(socketpair_ok);

    const uint8_t chn_id = 123;

    SmolRTSP_Transport tcp = smolrtsp_transport_tcp(smolrtsp_fd_writer, &fds[0], chn_id);

    const char total_len = strlen(DATA_0) + strlen(DATA_1);
    const char expected[] = {'$', chn_id, 0,   total_len, 'a', 'b', 'c',
                             'd', 'e',    'f', 'g',       'h', 'i'};

    CHECK_CALL(test_transport(tcp, fds[1], sizeof expected, expected));

    close(fds[0]);
    close(fds[1]);
    PASS();
}

TEST check_udp(void) {
    int fds[2];
    const bool socketpair_ok = socketpair(AF_UNIX, SOCK_SEQPACKET, 0, fds) == 0;
    ASSERT(socketpair_ok);

    SmolRTSP_Transport udp = smolrtsp_transport_udp(fds[0]);

    char expected[] = {DATA_0 DATA_1};

    CHECK_CALL(test_transport(udp, fds[1], strlen(expected), expected));

    close(fds[0]);
    close(fds[1]);
    PASS();
}

SUITE(transport) {
    RUN_TEST(check_tcp);
    RUN_TEST(check_udp);
}
