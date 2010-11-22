#include <fstream>

using namespace std;

int main()
{
  std::ifstream file("name", std::ios::nocreate);
  return 0;
}
