#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
/* Windows is pure evil */
#include <windows.h>
#else
#include <sys/socket.h>
#endif
#ifdef __cplusplus
}
#endif

int main()
{
    int i;
    struct sockaddr *addr = 0;
    int addrlen = 0;
    int optlen = 0;

    i = accept(1, addr, &addrlen);
    i = getsockopt(0, 0, 0, 0, &optlen);

    return 0;
}
