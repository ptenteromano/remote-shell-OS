#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main() {

  /* let's just be an ls program */

  execl("/bin/ls", "/bin/ls", "-l", NULL);

  /* is this line ever reached?  if yes, when? */

  puts("blah blah");
}