////////////////////////////////////////////////////////////////////////
//                                                                    //
// resource.h                                                         //
//                                                                    //
// definition of the resource, it provides members for the maintaining//
// of a resource                     				      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __RESOURCE_H
#define __RESOURCE_H

class histogram;

class resource
{
public :
  resource(double am);
  int report(histogram* hi,double intv = 0);
  void reset();
  void release(double am = 1.0);
  void acquire(double am = 1.0);
  friend ostream& operator<<(ostream& os,resource& r);

  // inline members, giving information of the state of the resource
  int available(double am = 1.0)
    { return (amount >= am); }
  int full()
    { return (amount >= initial); }
  int empty()
    { return (amount == 0.0); }
  double left()
    { return amount; }
  double used()
    { return (initial-amount); }
  double occupied()
    { return ((initial-amount) * 100.0/initial); }

private :
  double amount;   // current amount
  double initial;  // initial amount
  histogram* h;    // report on the occupation
};
  
#endif
