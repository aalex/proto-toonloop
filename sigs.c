#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int done = 0;

void termination_handler (int signum)
{
  /* struct temp_file *p;

  for (p = temp_file_list; p; p = p->next)
    unlink (p->name);
*/
  printf("Cleaning up\n");
  done = 1;
}

int main (void)
{
  printf("Starting\n");
  struct sigaction new_action, old_action;

  /* Set up the structure to specify the new action. */
  new_action.sa_handler = termination_handler;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;

  sigaction (SIGINT, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGINT, &new_action, NULL);
  sigaction (SIGHUP, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGHUP, &new_action, NULL);
  sigaction (SIGTERM, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGTERM, &new_action, NULL);

  while (done == 0) {
    printf(".");
    usleep(5000);
  }
  printf("Stopping\n");
  return 0;
}

