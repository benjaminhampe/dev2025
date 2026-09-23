#include "8z_TarTree.h"

struct UI_TarTree
{
    Fl_Tree* tree = nullptr;
};

static UI_TarTree ui;

TarTree::TarTree(std::string uri, int X, int Y, int W, int H)
    : Fl_Tree(X,Y,W,H,"TarTree")
{
    ui.tree = this;
    begin();
        add("Flintstones/Fred");
        add("Flintstones/Wilma");
        add("Flintstones/Pebbles");
        add("Simpsons/Homer");
        add("Simpsons/Marge");
        add("Simpsons/Bart");
        add("Simpsons/Lisa");
        add(uri.c_str());
    end();
}

TarTree::~TarTree()
{

}

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
