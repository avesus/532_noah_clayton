#include "watcher.h"


std::ostream& operator<<(std::ostream& os, const event_type ev)
{
  char buf[EV_MSG_LEN] = "";
  sprintf(buf,  "[%d]: %s\n", (int)ev, event_strings[(int)ev]);
  os << buf;
  return os;
}


std::ostream& operator<<(std::ostream& os, const event_info ei)
{
  char buf[EV_MSG_LEN] = "";
  sprintf(buf,
	  "Event[%d] {"
	  "\n\tEv Name: %s"
	  "\n\tTid: %lu"
	  "\n\tObj: %p"
	  "\n\tFunc: %p"
	  "\n\tOther: %p"
	  "\n\tFile: %s"
	  "\n\tLine: %d"
	  "\n\t}\n",
	  ei.ev,
	  event_strings[ei.ev],
	  ei.tid,
	  ei.obj,
	  ei.func,
	  ei.other,
	  ei.file,
	  ei.line);
  os << buf;
  return os;
  }

std::ostream& operator<<(event_logger& el, const event_info ei)
{
  
  std::cout << ei.obj << ": " << el.get_val(ei.obj) << std::endl;
  std::cout << ei.func << ": " << el.get_val(ei.func) << std::endl;
  std::cout << ei.other << ": " << el.get_val(ei.other) << std::endl;
  std::cout << ei.file << ": " << el.get_val(ei.file) << std::endl;
}
