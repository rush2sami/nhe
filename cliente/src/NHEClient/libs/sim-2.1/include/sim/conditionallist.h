////////////////////////////////////////////////////////////////////////
//                                                                    //
// conditionallist.h                                                  //
//                                                                    //
// headers for the list of conditional events, members for removing   //
// and for adding an event are provided. Adding an event will be in   //
// scheduling priority order.                                         //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __CONDITIONALLIST_H
#define __CONDITIONALLIST_H

class event;

ostream& list(ostream& os,event* p,int n);

class conditionallist
{
public :
  conditionallist();
  virtual ~conditionallist();
  void add(event* e);
  void remove(event* e);
  int cancel();
  event* head()
    { return hd; }
  event* tail()
    { return tl; }
  friend ostream& operator<<(ostream& os,conditionallist& c);

private :
  event* hd;  // head of the list
  event* tl;  // tail of the list
};

#endif
