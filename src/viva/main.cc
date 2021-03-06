/*
    This file is part of Viva.

    Viva is free software: you can redistribute it and/or modify it
    under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Viva is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Viva. If not, see <http://www.gnu.org/licenses/>.
*/
#include <Qt>
#include <PajeException.h>
#include "VivaApplication.h"
#include <GL/glut.h>
#include <argp.h>

static char doc[] = "Interactive and dynamic graph visualization of a Paje trace file";
static char args_doc[] = "<TRACEFILE> <VIVA_CONFIGURATION>";

static struct argp_option options[] = {
  { 0 }
};

static error_t parse_options (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = (struct arguments*)(state->input);
  switch (key){
  case ARGP_KEY_ARG:
    if (arguments->input_size == VALIDATE_INPUT_SIZE) {
      /* Too many arguments. */
      argp_usage (state);
    }
    arguments->input[state->arg_num] = arg;
    arguments->input_size++;
    break;
  case ARGP_KEY_END:
    if (state->arg_num < VALIDATE_INPUT_SIZE) {
      /* Not enough arguments. */
      argp_usage (state);
    }
    break;
  default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_options, args_doc, doc };

int main(int argc, char **argv)
{
  struct arguments arguments;
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr, "%s, error during the parsing of parameters\n", argv[0]);
    return 1;
  }

  glutInit (&argc, argv);
  QApplication::setAttribute( Qt::AA_X11InitThreads );
  VivaApplication app(argc, argv, &arguments);
  try {
    app.init();
  }catch (std::string exception){
    std::cout << "Exception: " << exception << std::endl;
    return 1;
  }catch (PajeException& exception){
    exception.reportAndExit ();
  }
  return app.exec();
}
