/*
    This file is part of Paje.

    Paje is free software: you can redistribute it and/or modify it
    under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Paje is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Paje. If not, see <http://www.gnu.org/licenses/>.
*/
#include "PajeEventDecoder.h"

static std::map<std::string,PajeEventId> knownPajeEvents = initPajeEventNamesToID ();
static PajeEventId getPajeEventId (std::string eventName)
{
  std::map<std::string,PajeEventId>::iterator it;
  if ((it = knownPajeEvents.find (eventName)) != knownPajeEvents.end()){
    return it->second;
  }else{
    return PajeUnknownEventId;
  }
}

PajeEventDecoder::PajeEventDecoder ()
{
  defStatus = OUT_DEF;
  currentLineNumber = 0;

  //TODO FIXME
  //chunkInfo ...
}

PajeEventDecoder::~PajeEventDecoder ()
{
  eventDefinitions.clear ();
}

char *PajeEventDecoder::break_line (char *s, paje_line *line)
{
  //a new line is born
  currentLineNumber++;

  bool in_string = false;
  bool in_word = false;
  char *p;
  line->word_count = 0;
  line->lineNumber = currentLineNumber;

  for (p = s; *p != '\0'; p++) {
    if (*p == '\n') {
      *p = '\0';
      p++;
      break;
    }
    if (in_string) {
      if (*p == '"') {
        *p = '\0';
        in_string = false;
      }
      continue;
    }
    if (*p == '#') {
      *p = '\0';
      while (true) {
        p++;
        if (*p == '\n') {
          p++;
          break;
        } else if (*p == '\0') {
          break;
        }
      }
      break;
    }
    if (in_word && isspace(*p)) {
      *p = '\0';
      in_word = false;
      continue;
    }
    if (!in_word && !isspace(*p)) {
      if (*p == '"') {
        p++;
        in_string = true;
      } else {
        in_word = true;
      }
      if (line->word_count < PAJE_MAX_FIELDS) {
        line->word[line->word_count] = p;
        line->word_count ++;
      }
      continue;
    }
  }
  return p;
}

void PajeEventDecoder::scanDefinitionLine (paje_line *line)
{
  char *str;
  int n = 0;
  char *eventName;
  char *eventId;
  char *fieldName;
  char *fieldType;
  std::stringstream st;
  st << *line;
  std::string lreport = st.str();

  str = line->word[n++];
  if (*str++ != '%') {
    throw std::string("Line should start with a '%%'");
  }
  if (*str == '\0') {
    str = line->word[n++];
  }

  switch (defStatus) {
  case OUT_DEF:
  {
    eventName = line->word[n++];
    eventId   = line->word[n++];

    //check if this event definition has a good start
    if (n != line->word_count || strcmp(str, "EventDef") != 0) {
      throw "'EventDef <event name> <event id>' expected in "+lreport;
    }

    //check if this event definition has been already defined
    if (eventDefinitions[eventId]){
      throw "Redefinition of event with id '"+std::string(eventId)+"' in "+lreport;
    }

    //check if we know the event name found in the trace file
    PajeEventId pajeEventId = getPajeEventId (eventName);
    if (pajeEventId == PajeUnknownEventId) {
      throw "Unknown event name '"+std::string(eventName)+"' in "+lreport;
    }

    eventBeingDefined = new PajeEventDefinition (pajeEventId, eventId, line);
    eventDefinitions[eventId] = eventBeingDefined;
    defStatus = IN_DEF;
  }
  break;
  case IN_DEF:
  {
    fieldName = str;

    if (n > line->word_count) {
      throw "Incomplete line, missing field name in "+lreport;
    }

    if (strcmp(fieldName, "EndEventDef") == 0) {
      if (!eventBeingDefined->isValid()){
        std::cout << "This event definition is invalid:" << std::endl;
        std::cout << *eventBeingDefined << std::endl;
        eventBeingDefined->showObligatoryFields();
        std::cout << "When treating line " << lreport << std::endl;
        exit(1);
      }
      defStatus = OUT_DEF;
      break;
    }

    if (n >= line->word_count) {
      std::map<PajeEventId,std::string> eventNames = initPajeEventIDToNames();
      std::string name = eventNames[eventBeingDefined->pajeEventId];

      throw "Incomplete line, missing field type for " + name + " with id " + " in "+lreport;
    }

    fieldType = line->word[n++];
    eventBeingDefined->addField (fieldName, fieldType, line);
  }
  break;
  default:
    throw std::string("Internal error, invalid status.");
  }
}

PajeEvent *PajeEventDecoder::scanEventLine (paje_line *line)
{
  char *eventId = NULL;
  PajeEventDefinition *eventDefinition = NULL;
  std::stringstream st;
  st << *line;
  std::string lreport = st.str();

  eventId = line->word[0];
  if (*eventId == '%') {
    throw "Line should not start with a '%%' in "+lreport;
  }
  eventDefinition = eventDefinitions[eventId];
  if (eventDefinition == NULL) {
    throw "Event with id '"+std::string(eventId)+"' has not been defined in "+lreport;
  }
  return new PajeEvent (eventDefinition, line);
}

void PajeEventDecoder::inputEntity (PajeObject *data)
{
  PajeData *d = (PajeData*)data;
  paje_line line;

  char *dataPointer = d->bytes;
  char *initDataPointer = d->bytes;
  int length = d->length;
  while ((dataPointer - initDataPointer) < length){
    dataPointer = PajeEventDecoder::break_line (dataPointer, &line);
    if (line.word_count == 0) {
      continue;
    }
    if (line.word[0][0] == '%') {
      PajeEventDecoder::scanDefinitionLine (&line);
    } else {
      PajeEvent *event = PajeEventDecoder::scanEventLine (&line);
      if (event != NULL){
        PajeComponent::outputEntity (event);
        delete event;
      }
    }
  }
}

bool PajeEventDecoder::canEndChunkBefore (PajeObject *data)
{
  PajeData *d = (PajeData*)data;
  paje_line line;

  bool canEndChunk = true;

  char *dataPointer = d->bytes;
  char *initDataPointer = d->bytes;
  int length = d->length;
  while ((dataPointer - initDataPointer) < length){
    dataPointer = PajeEventDecoder::break_line (dataPointer, &line);
    if (line.word_count == 0) {
      continue;
    }
    if (line.word[0][0] == '%') {
      PajeEventDecoder::scanDefinitionLine (&line);
      canEndChunk = false;
      break;
    }else{
      PajeEvent *event = PajeEventDecoder::scanEventLine (&line);
      if (event != NULL){
        canEndChunk = PajeComponent::canEndChunkBefore (event);
        delete event;
        break;
      }
    }
  }
  if (canEndChunk){
    return canEndChunk;
  }
  // the chunk cannot be ended -- process all remaining data
  while ((dataPointer - initDataPointer) < length) {
    dataPointer = break_line(dataPointer, &line);
    if (line.word_count == 0) {
      continue;
    }
    if (line.word[0][0] == '%') {
      PajeEventDecoder::scanDefinitionLine (&line);
    }else{
      PajeEvent *event = PajeEventDecoder::scanEventLine (&line);
      if (event != NULL){
        PajeComponent::outputEntity (event);
        delete event;
      }
    }
  }
  return false;
}