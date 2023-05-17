#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include "TextTable.h"

// Forward Decleration for Items and Perish classes
class Items;
class Perish;
class Date;
enum class Category;
class Date
{
    int day;
    int month;
    int year;

public:
    Date(int, int, int);
    Date();
    int isLeapYear();
    long long int convertToDays();
    void display();
    friend Date convertToDate(int);
    friend Date operator+(Date, int);
};
Date convertToDate(int);
class Storage
{
    std::vector<Items> items;
    std::vector<Perish> perish;
    std::vector<int> sales;
    int currentSales = 0;
    friend Items;
    friend Perish;

public:
    Date currentDate;
    void AddItems(Category category, std::string name, int price, bool Perishable);
    Storage();
    Items *FindObject(std::string key);
    std::vector<Items> retrieveItems(Category cat);
    void makeTable(std::vector<Items> vec);
    void makeTable(std::vector<Perish> vec);
    void makeTable(std::vector<int> vec);
    void Inventory();
    void handleRestock();
    void manageProfits();
    void manageRot();
    bool moveOntoNextDay();
} driver;
/////////////////////////////////////////////////////////////////////////////////////

//<-------------- Complete declaration of Date and related functions -------------------->
Date::Date(int a, int b, int c)
{
    day = a;
    month = b;
    year = c;
}
Date::Date()
{
    day = 0;
    month = 0;
    year = 0;
}
int Date::isLeapYear()
{
    if ((!year % 4 && year % 100) || year % 400)
    {
        return (1);
    }
    else
    {
        return (0);
    }
}
long long int Date::convertToDays()
{
    int daysInMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // calculates date upto the given date and then find the difference
    long long int daysFromDays, daysFromMonths, daysFromYears, totalDays;
    daysFromMonths = 0;
    daysFromYears = 0;
    daysFromDays = day;

    // determining days due to month
    if (month == 2 && isLeapYear())
        daysInMonth[2] = 29;
    for (int i = 0; i < month; i++)
    {
        daysFromMonths += daysInMonth[i];
    }

    // determining days due to years
    int daysInFourYears = 365 * 3 + 366;
    int year2 = year;
    daysFromYears += daysInFourYears * (year2 / 4); // deals with sets of 4 years that have passed
    // now to deal with leftover non leap years
    int sets = 4 * (daysFromYears / daysInFourYears);
    year2 -= sets; // now year2 holds number of non leap years to be accounted for
    daysFromYears += 365 * year2;

    totalDays = daysFromDays + daysFromMonths + daysFromYears;
    return totalDays;
}
void Date::display()
{
    std::cout << day << "/" << month << "/" << year << std::endl;
}
int operator-(Date d1, Date d2)
{
    return d1.convertToDays() - d2.convertToDays();
}
Date operator+(Date d, int n)
{
    Date d3;
    d3 = convertToDate(d.convertToDays() + n);
    return d3;
}
Date convertToDate(int days)
{
    Date d;
    int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int newDays = 0;
    int newMonths = 1;
    int newYear = 0;
    while (true)
    {
        // this part deals with number of years
        long long int daysInFourYears = 365 * 3 + 366;
        newYear += 4 * (days / daysInFourYears); // a set of 4 years from 0th year contains 365*3 + 366 days, this is what is accounts for
        days -= daysInFourYears * (newYear / 4);
        // to add the left over year:
        while (days >= 365)
        {
            newYear++;
            days -= 365;
        }
        d.year = newYear;
        // this part deals with figuring out the day and month
        // if after year calculation days = 0, this means it's 31st dec
        if (!days)
        {
            d.year--;
            d.day = 31;
            d.month = 12;
            break;
        }
        else
        {
            int monthCounter = 0; // to traverse the month array
            if (d.isLeapYear())
            {
                daysInMonth[1] = 29;
            }
            while (days > daysInMonth[monthCounter])
            {
                newMonths++;
                days -= daysInMonth[monthCounter++];
            }
            newDays = days;
        }
        d.day = newDays;
        d.month = newMonths;
        break;
    }
    return d;
}
//<-------------------------- End of declarations --------------------------------------->
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
        Quantity += quantity;
        int cost = quantity * price;
        driver.sales[driver.currentSales] -= cost;
    }
    friend Storage;
};
class Perish : public Items
{
    int Lifetime;
    std::vector<Date> dateOfManufacture;
    // batchQuantity[i] holds amount of perishible bought on dateOfManufacture[i]
    std::vector<int> batchQuantity;

public:
    Perish(Category cat, std::string name, int price, int quantity, int life, Date date)
        : Items(cat, name, price, quantity)
    {
        Lifetime = life;
        dateOfManufacture.push_back(date);
        batchQuantity.push_back(quantity);
    }
    void Restock(int quantity)
    {
        Quantity += quantity;
        driver.sales[driver.currentSales] -= quantity * price;
        batchQuantity.push_back(quantity);
    }
    std::pair<int, int> updateRot()
    {
        int perished = 0;
        int rottingTomorrow = 0;
        for (int i = 0; i < dateOfManufacture.size(); i++)
        {
            if (driver.currentDate - dateOfManufacture[i] > Lifetime)
            {
                dateOfManufacture.erase(dateOfManufacture.begin() + i);
                perished += batchQuantity[i];
                batchQuantity.erase(batchQuantity.begin() + i);
            }
            else if (driver.currentDate - dateOfManufacture[i] == Lifetime)
            {
                rottingTomorrow += batchQuantity[i];
            }
        }
        return {perished, rottingTomorrow};
    }
    void display(){
        std::cout<<Name<<std::endl;
    }
    friend Storage;
};

/////////////////////////////////////////////////////////////////////////////////////

//<-------------- Complete declaration of Storage and related functions -------------------->
// Enum for categories
// Perishables on top, Last entry as total for convenience
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
    switch (cat)
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
    if (cat == "Consumables")
        return Category::consumables;

    else if (cat == "Utensils")
        return Category::utensils;

    else if (cat == "Entertainment")
        return Category::entertainment;

    else if (cat == "Clothes")
        return Category::clothes;

    else if (cat == "Household")
        return Category::households;

    else
        return Category::total;
}
int getRandomNumber(int min, int max)
{
    // Generates Random number between max and min
    // Assumes std::srand() has already been called
    // assumes max-min <= RAND_MAX
    static constexpr double fraction{1.0 / (RAND_MAX + 1.0)}; // Static for efficiency

    // Evenly distributes random number across the range
    return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}
void Storage::AddItems(Category category, std::string name, int price, bool Perishable)
{
    // Method to Add Item to Vectors

    if (!Perishable)
    {
        items.push_back(Items(category, name, price, getRandomNumber(30, 50)));
    }
    else
    {
        perish.push_back(Perish(category, name, price, getRandomNumber(10, 30), 2, currentDate));
    }
}
Storage::Storage()
{
    currentDate = Date(getRandomNumber(1, 30), getRandomNumber(1, 12), getRandomNumber(2000, 2020));
    AddItems(Category::consumables, "Apples", 50, true);
    AddItems(Category::utensils, "Knifes", 150, false);
    AddItems(Category::entertainment, "Rubber_Balls", 150, false);
    AddItems(Category::clothes, "Shirts", 150, false);
    AddItems(Category::households, "Buckets", 150, false);
    AddItems(Category::consumables, "Tomatoes", 150, true);
    AddItems(Category::utensils, "Spoons", 150, false);
    AddItems(Category::entertainment, "Toy_Guns", 150, false);
    AddItems(Category::clothes, "Pants", 150, false);
    AddItems(Category::households, "Towels", 150, false);
    AddItems(Category::consumables, "Carrots", 150, true);
    AddItems(Category::utensils, "Forks", 150, false);
    AddItems(Category::entertainment, "Masks", 150, false);
    AddItems(Category::clothes, "Suits", 150, false);
    AddItems(Category::households, "Bottles", 150, false);

    sales.resize(static_cast<std::size_t>(Category::total));
}
Items *Storage::FindObject(std::string key)
{
    // CASE SENSITIVE
    for (int i = 0; i < items.size(); ++i)
    {
        if (items[i].Name == key)
        {
            return &items[i];
        }
    }
    for (int i = 0; i < perish.size(); ++i)
    {
        if (perish[i].Name == key)
        {
            return &perish[i];
        }
    }
    return NULL;
}
std::vector<Items> Storage::retrieveItems(Category cat)
{
    std::vector<Items> retrieval;
    for (int i = 0; i < items.size(); ++i)
    {
        if (items[i].cat == cat)
        {
            retrieval.push_back(items[i]);
        }
    }
    return retrieval;
}
void Storage::makeTable(std::vector<Items> vec)
{
    TextTable t('-', '|', '+');
    t.add("Item");
    t.add("Quantity");
    t.add("Price");
    t.endOfRow();

    for (int i = 0; i < vec.size(); ++i)
    {
        t.add(vec[i].Name);
        t.add(std::to_string(vec[i].Quantity));
        t.add(std::to_string(vec[i].price));
        t.endOfRow();
    }
    std::cout << t << std::endl;
}
void Storage::makeTable(std::vector<Perish> vec)
{

    std::vector<std::string> warning;
    TextTable t('-', '|', '+');
    t.add("Item");
    t.add("Quantity");
    t.add("Price");
    t.add("Lifetime");
    t.endOfRow();

    for (int i = 0; i < vec.size(); ++i)
    {
        t.add(vec[i].Name);
        t.add(std::to_string(vec[i].Quantity));
        t.add(std::to_string(vec[i].price));
        t.add(std::to_string(vec[i].Lifetime));
        t.endOfRow();

        if (vec[i].Lifetime - (currentDate - (vec[i].dateOfManufacture[0])) < 10)
        {
            warning.push_back(vec[i].Name);
        }
    }

    std::cout << t << std::endl;

    for (int i = 0; i < warning.size(); ++i)
    {
        std::cout << "A Batch of " << warning[i] << " has almost decayed.\n";
    }
}
void Storage::makeTable(std::vector<int> vec)
{
    TextTable t('-', '|', '+');
    t.add("Category");
    t.add("Sales");
    t.endOfRow();

    for (int i = 0; i < static_cast<int>(Category::total); ++i)
    {
        t.add(enumToStr(static_cast<Category>(i)));
        t.add(std::to_string(vec[i]));
        t.endOfRow();
    }
    std::cout << t << std::endl;
}
void Storage::Inventory()
{
    // function to print out categorywise profits
    for (int i = 1; i < static_cast<int>(Category::total); ++i)
    {
        std::cout << enumToStr(static_cast<Category>(i)) << std::endl;
        makeTable(retrieveItems(static_cast<Category>(i)));
    }
    std::cout << enumToStr(Category::consumables) << std::endl;
    makeTable(perish);
}
void Storage::handleRestock()
{
    int amount;
    std::cin >> std::ws >> amount;

    std::cout << "Do you want to restock a full category or only one item?(c/i) ";
    char choice;
    std::cin >> choice;

    if (choice == 'i')
    {
        std::cout << "Enter Name of object (Case Sensitive): ";
        std::string key;
        std::cin >> key >> std::ws;
        FindObject(key)->Restock(amount);
    }
    else if (choice == 'c')
    {
        std::cout << "Enter name of Category: ";
        std::string key;
        std::cin >> key;
        std::vector<Items> arr = retrieveItems(strToEnum(key));
        for (int i = 0; i < arr.size(); ++i)
        {
            FindObject(arr[i].Name)->Restock(amount);
        }
    }
}
void Storage::manageProfits()
{
    makeTable(sales);
}
void Storage::manageRot()
{
    int perished = 0;
    int rottingTomorrow = 0;

    for (int i = 0; i<perish.size(); i++)
    {
        perished += perish[i].updateRot().first;
        rottingTomorrow += perish[i].updateRot().second;
    }
    if (perished)
    {
        std::cout << "Number of units perished today: " << perished << std::endl;
    }
    if (rottingTomorrow)
    {
        std::cout << "Number of units perishing tomorrow: " << rottingTomorrow << std::endl;
    }
}
bool Storage::moveOntoNextDay()
{
    std::cout << "Do you want to continue? (y/n): ";
    char choice;
    std::cin >> choice;
    if (choice == 'n')
    {
        // int n = static_cast<int>(Category::total); // Its being used multiple times --> have added a separate member of class to keep track of current sales index
        int saleForTheDay = sales[currentSales];
        if (saleForTheDay < 0)
        {
            std::cout << "You have made a loss of " << -saleForTheDay << " total.\n";
        }
        else if (saleForTheDay > 0)
        {
            std::cout << "You have made a profit of " << saleForTheDay << " total.\n";
        }
        else
        {
            std::cout << "Congratulations!! You have gotten absolutely nothing!!\n";
        }
        return false;
    }
    else if (choice == 'y')
    {
        // currentDate + 1; -> this won't update the value of currentDate
        currentDate = currentDate + 1;
        currentSales++;
        // setting up sales for the next day
        sales.push_back(0);
        manageRot();
    }
}

//<-------------------------- End of declarations --------------------------------------->

int main()
{
    // // Seed rng Generator with system Clock and discard first value
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::rand();
    driver.Inventory();
    std::cout << "___________________________________________\n";
    driver.manageProfits();
    // make date private again
    driver.moveOntoNextDay();
    driver.Inventory();
    driver.currentDate.display();
    driver.moveOntoNextDay();
    driver.Inventory();
    driver.currentDate.display();
    driver.moveOntoNextDay();
    driver.Inventory();
    driver.currentDate.display();
    driver.moveOntoNextDay();
    driver.Inventory();
    driver.currentDate.display();
}