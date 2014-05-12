/*
 * Process.cpp
 *
 *  Created on: May 8, 2014
 *      Author: brianchin
 */

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include "cxx/process.h"
#include "cxx/exception.h"

using std::string;
using std::vector;

extern char **environ;

string CallExecutable(string const& path, vector<string> const& arguments) {
  int stdin_pipe[2] = {};
  if (pipe(stdin_pipe) < 0) {
    throw ProcessException(string("Couldn't create pipe: ") + strerror(errno));
  }

  pid_t child_pid = fork();

  if (child_pid < 0) {
    throw ProcessException(string("Couldn't fork: ") + strerror(errno));
  } else if (child_pid == 0) {
    // We're in the child. Close stdin and reopen stdout as the write end of
    // the pipe.
    dup2(stdin_pipe[1], STDOUT_FILENO);
    close(stdin_pipe[0]);
    close(STDIN_FILENO);
    close(stdin_pipe[1]);

    // Leave stderr pointing to the parent process stderr.

    char const** args = new char const*[arguments.size() + 1];
    for (int i = 0; i < arguments.size(); i++) {
      args[i] = arguments[i].c_str();
    }

    args[arguments.size()] = nullptr;

    execve(path.c_str(), (char * const *)args, environ);

    std::cout << "Couldn't execute child process: " << strerror(errno) << std::endl;
    exit(255);
  }

  // We're in the parent. Close the write end of the pipe.
  close(stdin_pipe[1]);

  // Set the read end of the pipe to be nonblocking
  int read_flags = fcntl(stdin_pipe[0], F_GETFL, 0);
  fcntl(stdin_pipe[0], F_SETFL, read_flags | O_NONBLOCK);

  // Read data from child process, and loop until the process closes.
  string child_output;
  int exit_code;
  while (waitpid(child_pid, &exit_code, WNOHANG) == 0) {
    char buffer[256];
    int num_read = read(stdin_pipe[0], buffer, 256);
    if (num_read >= 0) {
      child_output.append(buffer, buffer + num_read);
    } else if (num_read < 0 && errno != EAGAIN) {
      throw ProcessException(string("Couldn't read from pipe: ") + strerror(errno));
    }
  }

  return child_output;
}

