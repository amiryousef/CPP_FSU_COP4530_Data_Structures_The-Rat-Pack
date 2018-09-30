/*
    mazetest.cpp
    01/01/12
    Chris Lacher

    client program to test maze solver

    Copyright 2012, R.C. Lacher
*/

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <maze.h>

const int BATCH = 0;

const size_t maxFileNameSize(20);

int main()
{
  maze::Maze m;
  char mazefile [maxFileNameSize + 1];
  fsu::List<unsigned int> solution;
  std::cout << "\nWelcome to mazetest.\n";
  char entry;
  do
  {
    std::cout << "\nOptions:      1 = Initialize(), 2 = Consistent(), 3 = Solve(), 4 = Display(), 0 = Quit\n";
    std::cout << "Enter option: ";
    std::cin >> entry;
    if (BATCH) std::cout << entry << '\n';
    switch (entry)
    {
      case '1': case 'i': case 'I':
	std::cout << "Enter maze file name (0 to quit): ";
	std::cin >> std::setw(maxFileNameSize + 1) >> mazefile;
	if (BATCH) std::cout << mazefile << '\n';
	if (mazefile[0] == '0' && mazefile[1] == '\0')
	{
	  entry = '0';
	  break;
	}
	if (m.Initialize (mazefile))
	  std::cout << "** maze read successfully\n";
	break;
      case '2': case 'c': case 'C':
	if (m.Consistent()) 
	  std::cout << "** maze is self-consistent\n";
	break;
      case '3': case 's': case 'S':
        m.Solve(solution);
	if (solution.Empty())
	{
	  std::cout << "** no solution\n";
	}
	else
	{
	  std::cout << "** solution is:";
	  for (fsu::List<unsigned int>::Iterator i = solution.Begin(); i != solution.End(); ++i)
	    std::cout << ' ' << *i;
	  std::cout << '\n';
	}
	break;
      case '4': case 'd': case 'D':
	m.ShowMaze();
	break;
      case '5':
	// m.ShowMaze3D();
	break;
      case '0': case 'q': case 'Q': case 'x': case 'X':
	entry = '0';
	break;
      default:
	std::cout << "** bad entry - try again: ";
    }
  }
  while (entry != '0');

  std::cout << "\nThank you for using mazetest.\n\n";
  return EXIT_SUCCESS;
}
