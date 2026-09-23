#pragma once
#include <de/Core.h>
#include <FL/Fl_Tree.H>

struct TarTree : public Fl_Tree
{
    TarTree(std::string uri, int X, int Y, int W, int H);
    ~TarTree();

};

/*
Fl_Tree tree(X,Y,W,H);
tree.begin();
  tree.add("Flintstones/Fred");
  tree.add("Flintstones/Wilma");
  tree.add("Flintstones/Pebbles");
  tree.add("Simpsons/Homer");
  tree.add("Simpsons/Marge");
  tree.add("Simpsons/Bart");
  tree.add("Simpsons/Lisa");
tree.end();
*/
