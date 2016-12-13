#include <iomanip>
#include <ctime>
#include <fstream>
#include <sstream>

#ifdef DEBUG
void debug(const std::string &msg) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ofstream fout("/tmp/remotebuf.log", std::ios_base::app | std::ios_base::out);
  fout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << " :: " << msg << "\n";
}

void debug(const std::stringstream &sstm) {
  debug(sstm.str());
}
#else
void debug(const std::string &msg) {}
void debug(const std::stringstream &sstm) {}
#endif
