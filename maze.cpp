#include <iomanip>
#include <fstream>
#include <maze.h>

namespace maze
{
  Cell::Cell()
  {
    id_ = 0;
    visited_ = false;
    searchParent_ = 0;
  }

  Cell::~Cell()
  {
    Clear();
  }

  Cell::Cell(const Cell& c)
  {
    id_ = c.id_;
    neighborList_ = c.neighborList_;
    visited_ = c.visited_;
    searchParent_ = c.searchParent_;
  }

  Cell& Cell::operator=(const Cell& c)
  {
    if (this != &c)
    {
      Clear();
      id_ = c.id_;
      neighborList_ = c.neighborList_;
      visited_ = c.visited_;
      searchParent_ = c.searchParent_;
    }

    return *this;
  }

  void Cell::Clear()
  {
    id_ = 0;
    neighborList_.Clear();
    visited_ = false;
    searchParent_ = 0;
  }

  void Cell::SetID(unsigned int id)
  {
    id_ = id;
  }

  unsigned int Cell::GetID() const
  {
    return id_;
  }

  void Cell::SetParent(Cell* parent)
  {
    searchParent_ = parent;
  }

  Cell* Cell::GetParent() const
  {
    return searchParent_;
  }

  void Cell::SetVisited()
  {
    visited_ = true;
  }

  void Cell::UnSetVisited()
  {
    visited_ = false;
  }

  bool Cell::IsVisited() const
  {
    return visited_;
  }

  void Cell::AddNeighbor(Cell * N)
  {
    neighborList_.PushBack(N);
  }

  Cell* Cell::GetNextNeighbor() const
  {
    fsu::List<Cell*>::ConstIterator I;
    
    for (I = neighborList_.Begin(); I != neighborList_.End(); ++I)
    {
      if (!(*I)->IsVisited())
      {
        return *I;
      }
    }

    return 0;
  }

  bool Cell::IsNeighbor(const Cell * N) const
  {
    fsu::List<Cell*>::ConstIterator I;
    
    for (I = neighborList_.Begin(); I != neighborList_.End(); ++I)
    {
      if ((*I)->GetID() == N->GetID())
      {
        return true;
      }
    }

    return false;
  }

  Maze::Maze()
  {
    numrows_ = 0;
    numcols_ = 0;
    start_ = 0;
    goal_ = 0;
  }

  Maze::~Maze()
  {
    Clear();
  }

  void Maze::Clear()
  {
    fsu::Vector<Cell>::Iterator I;
    
    for (I = cellVector_.Begin(); I != cellVector_.End(); ++I)
    {
      (*I).Clear();
    }
    
    numrows_ = 0;
    numcols_ = 0;
    start_ = 0;
    goal_ = 0;
    cellVector_.Clear();
    conQueue_.Clear();
  }

  bool Maze::Initialize(const char* filename)
  {
    std::ifstream ifs;
    unsigned int size;
    unsigned int number;
    unsigned int row;
    unsigned int col;
    unsigned int walls;
    unsigned int start;
    unsigned int goal;

    Clear();

    // open file
    ifs.open(filename);

    // check for open file
    if (!ifs)
    {
      std::cerr << "** ERROR: unable to open file " << filename << '\n';
      Clear();
      ifs.close();
      return false;
    }

    // read dimensions
    if (!(ifs >> numrows_) || !(ifs >> numcols_))
    {
      std::cerr << "** DEFECT: Maze::Initialize(): unable to read size data from " << filename << '\n';
      Clear();
      ifs.close();
      return false;
    }

    // establish size of cell inventory
    size = numrows_ * numcols_;
    cellVector_.SetSize(size);

    // loop
    for (number = 0; number < size; number++)
    {
      row = number / numcols_;
      col = number % numcols_;

      // read walls code
      if (!(ifs >> walls))
      {
        std::cerr << "** DEFECT: unable to read walls code [" << number << "]  from " << filename << '\n';
        Clear();
        ifs.close();
        return false;
      }
      
      // walls code out of range
      if (walls > 15)
      {
        std::cerr << "** DEFECT: walls code [" << number << "] out of range in file " << filename << '\n';
        Clear();
        ifs.close();
        return false;
      }

      // fails to specify a wall at a north face maze boundary
      if ((row == 0) && ((walls & 0x01) == 0))
      {
        std::cerr << "** DEFECT: walls code [" << number << "] repaired at North face maze boundary " << filename << '\n';
      }
      
      // fails to specify a wall at a east face maze boundary
      if (((col + 1) == numcols_) && ((walls & 0x02) == 0))
      {
        std::cerr << "** DEFECT: walls code [" << number << "] repaired at East face maze boundary " << filename << '\n';
      }
  
      // fails to specify a wall at a south face maze boundary
      if (((row + 1) == numrows_) && ((walls & 0x04) == 0))
      {
        std::cerr << "** DEFECT: walls code [" << number << "] repaired at South face maze boundary " << filename << '\n';
      }
    
      // fails to specify a wall at a west face maze boundary
      if ((col == 0) && ((walls & 0x08) == 0))
      {
        std::cerr << "** DEFECT: walls code [" << number << "] repaired at West face maze boundary " << filename << '\n';
      }

      // set up neighbor list
      if ((row > 0) && ((walls & 0x01) == 0))
      {
        cellVector_[number].AddNeighbor(&cellVector_[number - numcols_]);
      }
      
      if (((col + 1) < numcols_) && ((walls & 0x02) == 0))
      {
        cellVector_[number].AddNeighbor(&cellVector_[number + 1]);
      }

      if (((row + 1) < numrows_) && ((walls & 0x04) == 0))
      {
        cellVector_[number].AddNeighbor(&cellVector_[number + numcols_]);
      }

      if ((col > 0) && ((walls & 0x08) == 0))
      {
        cellVector_[number].AddNeighbor(&cellVector_[number - 1]);
      }

      // set ID of cell
      cellVector_[number].SetID(number);
    }

    // read start, goal
    if (!(ifs >> start) || !(ifs >> goal))
    {
      std::cerr << "** DEFECT: unable to read start/goal data from " << filename << '\n';
      Clear();
      ifs.close();
      return false;
    }

    // start, goal out of range
    if (start >= size || goal >= size)
    {
      std::cerr << "** DEFECT: bad start/goal data read from " << filename << '\n';
      Clear();
      ifs.close();
      return false;
    }

    // close file
    ifs.close();

    start_ = &cellVector_[start];
    goal_ = &cellVector_[goal];

    return true;
  }

  bool Maze::Consistent() const
  {
    unsigned int size = numrows_ * numcols_;
    unsigned int number;
    fsu::List<Cell*>::ConstIterator I;
    bool result = true;

    for (number = 0; number < size; number++)
    {
      for (I = cellVector_[number].neighborList_.Begin(); I != cellVector_[number].neighborList_.End(); ++I)
      {
        if (!(*I)->IsNeighbor(&cellVector_[number]))
        {
          std::cerr << "** DEFECT: neighbor asymetry between cells " << number << " and " << (*I)->GetID() << '\n';
          result = false;
        }
      }
    }

    return result;
  }

  void Maze::Solve(fsu::List<unsigned int>& solution)
  {
    Cell* front;
    Cell* N;

    // clear conQueue
    conQueue_.Clear();
    solution.Clear();
    
    if (start_ == 0)
    {
      std::cerr << "** no solution -- invalid start\n";
      return;
    }

    // for all maze cells, unset visited flag and 
    // initialize backtrack pointer to 0
    for (unsigned int number = 0; number < numrows_ * numcols_; number++)
    {
      cellVector_[number].UnSetVisited();
      cellVector_[number].SetParent(0);
    }

    // set visited flag for start cell
    start_->SetVisited();
    // push start onto conQueue
    conQueue_.Push(start_);

    // while conQueue is not empty
    while (!conQueue_.Empty())
    {
      // get the front cell
      front = conQueue_.Front();

      // if front cell is the goal cell, then
      // populate solution using backtract pointers
      if (front == goal_)
      {
        N = goal_;

        while (N != 0)
        {
          solution.PushFront(N->GetID());
          N = N->GetParent();
        }

        break;
      }

      // while front cell has unvisited neighbor N
      while ((N = front->GetNextNeighbor()) != 0)
      {
        // set N's visited flag
        N->SetVisited();
        // set N's backtrack pointer
        N->SetParent(front);
        // push address of N onto the conQueue
        conQueue_.Push(N);
      }

      // pop the conQueue
      conQueue_.Pop();
    }
  }

  void Maze::ShowMaze(std::ostream& os) const
  {
    unsigned int size = numrows_ * numcols_;
    unsigned int number;
    unsigned int row;
    unsigned int col;

    if (size == 0)
    {
      os << "\n[empty maze]\n";
      return;
    }

    for (col = 0; col < numcols_; col++)
      os << " _";
    os << "\n";

    for (number = 0; number < size; number++)
    {
      row = number / numcols_;
      col = number % numcols_;

      if (col == 0 || !cellVector_[number].IsNeighbor(&cellVector_[number - 1]))
        os << "|";
      else
        os << " ";

      if ((row + 1) == numrows_ || !cellVector_[number + numcols_].IsNeighbor(&cellVector_[number]))
        os << "_";
      else
        os << " ";

      if ((col + 1) == numcols_)
        os << "|\n";
    }

    os << "\n";
    os << " start cell: " << std::setw(2) << start_->GetID();
    os << " [" << start_->GetID() / numcols_ << "," << start_->GetID() % numcols_ << "]\n";
    os << "  goal cell: " << std::setw(2) << goal_->GetID();
    os << " [" << goal_->GetID() / numcols_ << "," << goal_->GetID() % numcols_ << "]\n";
  }
}
