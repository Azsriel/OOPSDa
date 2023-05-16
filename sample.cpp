#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include "TextTable.h"
using namespace std;

// Forward Decleration for Items and Perish classes
class Items;
class Perish;
class Date;
enum class Category;
class Storage;

/////////////////////////////////////////////////////////////////////////////////////
// Temporary Date class to remove Errors
// REMOVE LATER
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
    int isLeapYear()
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
    long long int convertToDays()
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
        // cout<<"The value of year2: "<<year2<<endl;
        int sets = 4 * (daysFromYears / daysInFourYears);
        // cout<<"The value of sets: "<<sets<<endl;
        year2 -= sets; // now year2 holds number of non leap years to be accounted for
        // cout<<"The value of daysFromYears: "<<daysFromYears<<endl;
        // cout<<"The value of year2: "<<year2<<endl;
        daysFromYears += 365 * year2;
        // cout<<"The value of daysFromYears: "<<daysFromYears<<endl;

        totalDays = daysFromDays + daysFromMonths + daysFromYears;
        return totalDays;
    }
    void display()
    {
        cout << day << "/" << month << "/" << year << endl;
    }
    // friend int operator-(Date d1, Date d2);
    friend Date operator+(Date d1, int n);
    friend Date convertToDate(int);
};

int operator-(Date d1, Date d2)
{
    return d1.convertToDays() - d2.convertToDays();
}
Date operator+(Date d, int n)
{
    Date d3;
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
int main()
{
    Date d1(31, 3, 2023);
    cout << d1.convertToDays() << endl;
    Date d3;
    d3 = convertToDate(d1.convertToDays());
    d3.display();
}