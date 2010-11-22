#ifdef __cplusplus
extern "C" {
#endif
#include <sys/socket.h>
#ifdef __cplusplus
}
#endif

int main()
{
    int i;
    int fds = 0;

    i = select(1, &fds, &fds, &fds, 0);

    return 0;
}
