#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include "TextTable.h"

//Forward Decleration for Items and Perish classes
class Items;
class Perish;
class Date;
enum class Category;
class Storage;

/////////////////////////////////////////////////////////////////////////////////////
//Temporary Date class to remove Errors
//REMOVE LATER
class Date
{
    int day;
    int month;
    int year;

    public:
    Date(int a, int b, int c)
    {
        day = a;
        month = b;
        year = c;
    }
    Date()
    {
        day = 0;
        month = 0;
        year = 0;
    }
    friend int operator-(Date d1, Date d2);
    friend void operator+(Date d1, int n);
};

int operator-(Date d1, Date d2)
{
    return 5;
}
void operator+(Date d1, int n)
{
    d1.day++;
}

//Temporary items and Perish Classes to remove Errors
//REMOVE LATER
class Items
{
    protected:
    Category cat;
    std::string Name;
    int price;
    int Quantity;

    public:
    Items(Category cat, std::string Name, int price, int Quantity)
    {
        this->cat = cat;
        this->Name = Name;
        this->price = price;
        this->Quantity = Quantity;
    }
    void Restock(int quantity)
    {
        Quantity += 500;
    }
    friend Storage;
};

class Perish:public Items
{
    int Lifetime;
    std::vector<Date> dateOfManufacture;

    public:
    Perish(Category cat, std::string name, int price, int quantity, int life, Date date)
    : Items(cat,name,price,quantity)
    {
        Lifetime = life;
        dateOfManufacture.push_back(date);
    }
    void Restock(int quantity);
    friend Storage;
};

//END REMOVE HERE
/////////////////////////////////////////////////////////////////////////////////////

//Enum for categories
//Perishables on top, Last entry as total for convenience
enum class Category 
{
    consumables,
    utensils,
    entertainment,
    clothes,
    households,
    total,
};

std::string enumToStr(Category cat)
{
    switch(cat)
    {
        case Category::consumables:
            return "Consumables";
        case Category::utensils:
            return "Utensils";
        case Category::entertainment:
            return "Entertainment";
        case Category::clothes:
            return "Clothes";
        case Category::households:
            return "Household";
        default:
            return "???";
    }
}
Category strToEnum(std::string cat)
{
    if(cat == "Consumables") return Category::consumables;

    else if(cat == "Utensils") return Category::utensils;

    else if(cat == "Entertainment") return Category::entertainment;

    else if(cat == "Clothes") return Category::clothes;

    else if(cat == "Household") return Category::households;

    else return Category::total;
    
}


//Generates Random number between max and min
//Assumes std::srand() has already been called
//assumes max-min <= RAND_MAX
int getRandomNumber(int min, int max)
{
    static constexpr double fraction {1.0/ (RAND_MAX + 1.0)}; //Static for efficiency
    
    //Evenly distributes random number across the range
    return min + static_cast<int>((max-min+1) * (std::rand() * fraction));
}


class Storage
{
    //Vector for storing all objects of Item Class
    std::vector<Items> items;

    //Vector for storing all objects of Perish Class
    std::vector<Perish> perish;

    //Vector for storing all Sales with <int>Category serving as index
    std::vector<int> sales;

    //Current Date
    Date currentDate;

    //Make the item classes as friends
    friend Items;
    friend Perish;

    public:
    //Method to Add Item to Vectors
    void AddItems(Category category, std::string name, int price, bool Perishable)
    {
        if(!Perishable)
        {
            items.push_back(Items(category,name,price,getRandomNumber(30,50)));
        }
        else
        {
            perish.push_back(Perish(category,name,price,getRandomNumber(10,30),10,currentDate));
        }
    }

    //Constructor
    Storage()
    {
        currentDate = Date(getRandomNumber(1,30),getRandomNumber(1,12),getRandomNumber(2000,2020));
        AddItems(Category::consumables,"Apples",50,true);
        AddItems(Category::utensils,"Knifes",150,false);
        AddItems(Category::entertainment,"Rubber_Balls",150,false);
        AddItems(Category::clothes,"Shirts",150,false);
        AddItems(Category::households,"Buckets",150,false);
        AddItems(Category::consumables,"Tomatoes",150,true);
        AddItems(Category::utensils,"Spoons",150,false);
        AddItems(Category::entertainment,"Toy_Guns",150,false);
        AddItems(Category::clothes,"Pants",150,false);
        AddItems(Category::households,"Towels",150,false);
        AddItems(Category::consumables,"Carrots",150,true);
        AddItems(Category::utensils,"Forks",150,false);
        AddItems(Category::entertainment,"Masks",150,false);
        AddItems(Category::clothes,"Suits",150,false);
        AddItems(Category::households,"Bottles",150,false);

        sales.resize(static_cast<std::size_t>(Category::total));
    }

    //CASE SENSITIVE
    Items* FindObject(std::string key)
    {
        for(int i = 0; i<items.size(); ++i)
        {
            if(items[i].Name == key)
            {
                return &items[i];
            }
        }
        for(int i = 0; i<perish.size(); ++i)
        {
            if(perish[i].Name == key)
            {
                return &perish[i];
            }
        }
        return NULL;
    }

    std::vector<Items> retrieveItems(Category cat)
    {
        std::vector<Items> retrieval;
        for(int i = 0; i<items.size(); ++i)
        {
            if(items[i].cat == cat)
            {
                retrieval.push_back(items[i]);
            }
        }
        return retrieval;
    }

    void makeTable(std::vector<Items> vec)
    {
        TextTable t( '-', '|', '+' );
        t.add("Item");
        t.add("Quantity");
        t.add("Price");
        t.endOfRow();

        for(int i = 0; i< vec.size(); ++i)
        {
            t.add(vec[i].Name);
            t.add(std::to_string(vec[i].Quantity));
            t.add(std::to_string(vec[i].price));
            t.endOfRow();
        }
        std::cout << t << std::endl;
    }

    void makeTable(std::vector<Perish> vec)
    {

        std::vector<std::string> warning;
        TextTable t( '-', '|', '+' );
        t.add("Item");
        t.add("Quantity");
        t.add("Price");
        t.add("Lifetime");
        t.endOfRow();

        for(int i = 0; i< vec.size(); ++i)
        {
            t.add(vec[i].Name);
            t.add(std::to_string(vec[i].Quantity));
            t.add(std::to_string(vec[i].price));
            t.add(std::to_string(vec[i].Lifetime));
            t.endOfRow();

            if(vec[i].Lifetime - (currentDate - (vec[i].dateOfManufacture[0])) < 10)
            {
                warning.push_back(vec[i].Name);
            }
        }

        std::cout << t << std::endl;

        for(int i = 0; i<warning.size(); ++i)
        {
            std::cout << "A Batch of " << warning[i] << " has almost decayed.\n";
        }
    }
    void makeTable(std::vector<int> vec)
    {
        TextTable t( '-', '|', '+' );
        t.add("Category");
        t.add("Sales");
        t.endOfRow();

        for(int i = 0; i< static_cast<int>(Category::total); ++i)
        {
            t.add(enumToStr(static_cast<Category>(i)));
            t.add(std::to_string(vec[i]));
            t.endOfRow();
        }
        std::cout << t << std::endl;
    }

    void Inventory()
    {
        for(int i = 1; i< static_cast<int>(Category::total); ++i)
        {
            std::cout << enumToStr(static_cast<Category>(i)) << std::endl;
            makeTable(retrieveItems(static_cast<Category>(i)));
        }
        std::cout << enumToStr(Category::consumables) << std::endl;
        makeTable(perish);
    }

    void handleRestock()
    {
        int amount;
        std::cin >> std::ws >> amount;

        std::cout << "Do you want to restock a full category or only one item?(c/i) ";
        char choice;
        std::cin >> choice;

        if(choice == 'i')
        {
            std::cout << "Enter Name of object (Case Sensitive): ";
            std::string key;
            std::cin >> key >> std::ws;
            FindObject(key)->Restock(amount);
        }
        else if(choice == 'c')
        {
            std::cout << "Enter name of Category: ";
            std::string key;
            std::cin >> key;
            std::vector<Items> arr = retrieveItems(strToEnum(key));
            for(int i = 0; i<arr.size(); ++i)
            {
                FindObject(arr[i].Name)->Restock(amount);
            }

        }
    }

    void manageProfits()
    {
        makeTable(sales);
    }
    void manageRot();

     bool moveOntoNextDay()
    {
        std::cout << "Do you want to continue? (y/n): ";
        char choice;
        std::cin >> choice;
        if(choice == 'n')
        {
            int n = static_cast<int>(Category::total); //Its being used multiple times
            if(sales[n] < 0)
            {
                std::cout << "You have made a loss of " << -n << " total.\n"; 
            }
            else if(sales[n] > 0)
            {
                std::cout << "You have made a profit of " << n << " total.\n"; 
            }
            else
            {
                std::cout << "Congratulations!! You have gotten absolutely nothing!!\n";
            }
            return false;
        }
        else if(choice == 'y')
        {
            currentDate + 1;

            manageRot();
        }

    }

    


};



int main()
{
    //Seed rng Generator with system Clock and discard first value
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::rand();

    Storage driver;
    driver.Inventory();
    std::cout << "___________________________________________\n";
    driver.manageProfits();

}