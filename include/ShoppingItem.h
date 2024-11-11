#ifndef SHOPPINGITEM_H
#define SHOPPINGITEM_H

#include <string>
#include <vector>

using namespace std;

class ShoppingItem
{
public:
    void setName(const string &name);
    void setQuantity(int quantity);
    void setAcquired(bool acquired);
    string getName() const;
    int getQuantity() const;
    bool isAcquired() const;

private:
    string name;
    int quantity;
    bool acquired = false;
};

#endif // SHOPPINGITEM_H
