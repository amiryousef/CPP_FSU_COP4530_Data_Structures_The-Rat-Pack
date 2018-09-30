/*
    ratpack.cpp
    01/01/12
    Chris Lacher

    client maze solver

    Copyright 2012
*/

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <maze.h>

const size_t maxFileNameSize(20);

/*
  NOTICE: UNCOMMENT LINES 38 AND 40 IF Consistent() EXISTS
          UNCOMMENT LINES 53 AND 54 IF ShowMaze() EXISTS
*/

int main()
{
   maze::Maze m;
   char mazefile [maxFileNameSize + 1];
   fsu::List<unsigned int> solution;
   std::cout << "Welcome to ratpack, a maze solving program.\n";
   while(1)
   {
     std::cout << "\nEnter maze file name (0 to quit): ";
     std::cin >> std::setw(maxFileNameSize + 1) >> mazefile;
     if (mazefile[0] == '0' && mazefile[1] == '\0')
       break;
     if (m.Initialize (mazefile))
     {
       std::cout << "** maze read successfully\n";
       if (m.Consistent())                              /*38*/
       {
         std::cout << "** maze is self-consistent\n";   /*40*/
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
         std::cout << "** maze graphic:\n";             /*53*/
         m.ShowMaze();                                  /*54*/
       }
     }
     else
       std::cout << "** try again\n";
   }

   std::cout << "\nThank you for using ratpack.\n\n";
   return EXIT_SUCCESS;
}
