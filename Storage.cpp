#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <limits>
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
    void handleAddItems();
    Storage();
    Items *FindObject(std::string key);
    std::vector<Items> retrieveItems(Category cat);
    void makeTable(std::vector<Items> vec);
    void makeTable(std::vector<Perish> vec);
    void makeTable(std::vector<int> vec);
    void manageSales();
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
    virtual int Sales(int units)
    {
        if (Quantity < units)
        {
            return 0;
        }
        Quantity -= units;
        driver.sales[driver.currentSales] += units * price;
        if (Quantity < 10)
        {
            std::cout << Name << " running low!! restock suggested!!" << std::endl;
        }
        return 1;
    }
    virtual void Restock(int quantity)
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
        //Quantity += quantity; not needed because items already assigns quantity
    }
    int Sales(int units)
    {
        int tempUnits = units;
        std::vector<int> tempBatchQuantity = batchQuantity;
        while (true)
        {
            if (!tempBatchQuantity.size())
            {
                return 0;
            }
            if (tempUnits >= tempBatchQuantity[0])
            {
                tempUnits -= tempBatchQuantity[0];
                tempBatchQuantity.erase(tempBatchQuantity.begin());
            }
            else
            {
                tempBatchQuantity[0] -= tempUnits;
                tempUnits = 0;
            }
            if (!tempUnits)
            {
                batchQuantity = tempBatchQuantity;
                driver.sales[driver.currentSales] += units * price;
                Quantity -= units;
                if (Quantity < 10)
                {
                    std::cout << Name << " running low!! restock suggested!!" << std::endl;
                }
                return 1;
            }
        }
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
                Quantity -= perished;
            }
            else if (driver.currentDate - dateOfManufacture[i] == Lifetime)
            {
                rottingTomorrow += batchQuantity[i];
            }
        }
        return {perished, rottingTomorrow};
    }
    void display()
    {
        std::cout << Name << std::endl;
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

void Storage::handleAddItems()
{
    std::string cat,name;
    int price = 0;
    char choice;
    bool isPerish;

    std::cout << "Enter Category that Item belongs to: ";
    std::cin >> std::ws >> cat;
    while(strToEnum(cat) == Category::total)
    {
        std::cout << "Invalid Input. Please enter a valid Category: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        //Clears console buffer till \n     
        std::cin >> cat;     
    }
    std::cin.ignore(1000, '\n');

    std::cout << "Enter Name of Item(One word): ";
    std::cin >> std::ws >> name;
    std::cin.ignore(1000, '\n');

    std::cout << "Enter Price of the Item: ";
    std::cin >> price;
    while(price <= 0)
    {
        std::cout << "Invalid Input. Please enter a positive price: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        //Clears console buffer till \n     
        std::cin >> price;     
    }
    std::cin.ignore(1000, '\n');
    

    std::cout << "Is a/an " << name << " perishable?(y/n): ";
    std::cin >> choice;
    //Error handling
    while (std::cin.fail() || choice != 'n' || choice != 'N' || choice != 'y' || choice != 'Y')
    {
        std::cout << "Invalid Input. Please enter only the characters 'y','Y','n' or 'N':  ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        //Clears console buffer till \n
        std::cin >> choice;
    }
    std::cin.ignore(1000, '\n');

    if(choice == 'y' || choice == 'Y')
        isPerish = true;
    else if (choice == 'n' || choice == 'N');
        isPerish = false;

    AddItems(strToEnum(cat),name,price,isPerish);
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
        t.add(std::to_string(vec[i].Lifetime) + " days");
        t.endOfRow();

        
        if (vec[i].Quantity > 0 && vec[i].Lifetime - (currentDate - (vec[i].dateOfManufacture[0])) < 10)
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
void Storage::manageSales()
{
    int n = 0;
    int qty = 0;
    std::string name;
    std::cout << "Enter the number(types not quantity) of items sold: ";
    std::cin >> n;
    while (n <= 0)
    {
        std::cout << "Invalid Input.Please enter a valid positive Number: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        //Clears console buffer till \n
        std::cin >> n;
    }
    std::cin.ignore(1000, '\n');

    for(int i = 0; i< n; ++i)
    {
        std::cout << "Enter the name of the item: ";
        std::cin >> name;
        Items* ptr = FindObject(name);
        while(!ptr)
        {
            std::cout << "Invalid Input. Please enter valid Item name: ";
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            //Clears console buffer till \n     
            std::cin >> name;
            ptr = FindObject(name);
        }
        std::cin.ignore(1000, '\n');

        std::cout << "Enter quantity of '" << name << "' sold: ";
        std::cin >> qty;
        while (qty <= 0 || qty > ptr->Quantity)
        {
            if(qty > ptr->Quantity)
            {
                std::cout << "Invalid Input. Quantity entered is greater than existing amount.\n";
                std::cout << "Please enter a valid quantity: ";          
            }
            else
            {
                std::cout << "Invalid Input. Please enter a valid Positive quantity: ";
            }
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            //Clears console buffer till \n     
            std::cin >> qty;     
        }
        std::cin.ignore(1000, '\n');

        ptr->Sales(qty);
    }
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
    std::cout << "Enter amount needed to restock: ";
    std::cin >> amount;
    while(amount <= 0)  
    {  
        std::cout << "Invalid Input. Please enter a valid Positive quantity: ";  
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        //Clears console buffer till \n     
        std::cin >> amount;
    }
    std::cin.ignore(1000, '\n');


    std::cout << "Do you want to restock a full category or only one item?(c/i) ";
    char choice;
    std::cin >> choice;
    //Error handling
    while (choice != 'i' && choice != 'I' && choice != 'c' && choice != 'C')
    {
        std::cout << "Invalid Input. Please enter only the characters 'i','I','c' or 'C':  ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        //Clears console buffer till \n
        std::cin >> choice;
    }
    std::cin.ignore(1000, '\n');

    if (choice == 'i' || choice == 'I')
    {
        std::cout << "Enter Name of object (Case Sensitive): ";
        std::string key;
        std::cin >> key;
        Items* ptr = FindObject(key);
        while(!ptr)
        {
            std::cout << "Invalid Input. Please enter valid Item name: ";
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            //Clears console buffer till \n
            std::cin >> key;
            ptr = FindObject(key);
        }
        std::cin.ignore(1000, '\n');

        ptr->Restock(amount);
    }
    else if (choice == 'c' || choice == 'C')
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
    //No else required
}
void Storage::manageProfits()
{
    makeTable(sales);
}
void Storage::manageRot()
{
    int perished = 0;
    int rottingTomorrow = 0;

    for (int i = 0; i < perish.size(); i++)
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
    //Error handling
    while (choice != 'n' && choice != 'N' && choice != 'y' && choice != 'Y')
    {
        std::cout << "Invalid Input. Please enter only the characters 'y','Y','n' or 'N':  ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        //Clears console buffer till \n
        std::cin >> choice;
    }
    std::cin.ignore(1000, '\n');

    if (choice == 'n' || choice == 'N')
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
        return true;
    }
    else if (choice == 'y' || choice == 'Y')
    {
        currentDate = currentDate + 1;
        currentSales++;
        // setting up sales for the next day
        sales.push_back(0);
        manageRot();
        return false;
    }
    //not needed but warnings are treated as errors
    return false;
    
}

//<-------------------------- End of declarations --------------------------------------->

int main()
{
    // // Seed rng Generator with system Clock and discard first value
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::rand();

    bool completed = false;
    while(!completed){
        std::cout<<"\nMain Menu:"<<std::endl;
        std::cout<<"1) Customer Sales"<<std::endl;
        std::cout<<"2) See Inventory"<<std::endl;
        std::cout<<"3) Restock"<<std::endl;
        std::cout<<"4) Calculate Profits"<<std::endl;
        std::cout<<"5) Add Item to Inventory"<<std::endl;
        std::cout<<"6) Move onto Next day"<<std::endl;
        std::cout<<"Enter choice :"<<std::endl;
        int choice;
        std::cin>>choice;
        switch(choice){
        case 1:
            driver.manageSales();
            break;
        case 2:
            driver.Inventory();
            break;
        case 3:
            driver.handleRestock();
            break;
        case 4:
            driver.manageProfits();
            break;
        case 5:
            driver.handleAddItems();
            break;
        case 6:
            completed = driver.moveOntoNextDay();
            break;
        default:
            std::cout<<"Invalid input , kindly enter number from 1 to 6."<<std::endl;
            break;
        }
    }  
}
