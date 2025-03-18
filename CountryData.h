#ifndef COUNTRY_DATA_H
#define COUNTRY_DATA_H

#include <string>

static const int MAX_COUNTRIES = 512;

// Slot status values (using simple integers)
static const int STATUS_EMPTY = 0;
static const int STATUS_OCCUPIED = 1;
static const int STATUS_PREV_OCCUPIED = 2;

class CountryData {
private:
    // 1) Series: Represents a time series record for a country.
    struct Series {
        std::string seriesName;
        std::string seriesCode;
        int *years;
        double *values;
        int numEntries;
        int maxEntries;
        Series *next; // singly-linked list

        Series(const std::string &name, const std::string &code);
        ~Series();
        void resize();
    };

    // 2) CountryNode: Represents one country and its data.
    struct CountryNode {
        std::string countryName;
        std::string countryCode;
        Series *timeSeries; // linked list of series records

        CountryNode(const std::string &name, const std::string &code);
        ~CountryNode();
    };

    // 3) Entry: Used for the BUILD-related commands.
    struct Entry {
        std::string countryName;
        double mean;  // computed as (sum of valid values)/(# valid values) or 0 if none
    };

    // data Members 

    // Hash table (array of CountryNode pointers) and an accompanying slot status array.
    CountryNode *countryArray[MAX_COUNTRIES];
    int slotStatus[MAX_COUNTRIES];
    int countryCount;

    // for Project 3 commands that originally used a tree, we now build a dynamic array.
    // This array is built by the BUILD command and used by RANGE, FIND, LIMITS, and DELETE (by country name).
    Entry **buildEntries;
    int buildSize;
    int buildCapacity;  
    std::string lastBuiltSeries;

    // --- Hashing Helper Methods ---
    unsigned int codeToInteger(const std::string &code) const;
    unsigned int h1(unsigned int W) const;
    unsigned int h2(unsigned int W) const;
    bool hashInsert(CountryNode *newCountry);
    std::pair<int,int> hashSearch(const std::string &code) const;
    bool hashRemove(const std::string &code);

    // --- Helper Methods for BUILD & Related Commands (no trees, just linear arrays) ---
    // Build a dynamic array of Entry pointers (one per country with the specified series code).
    bool buildStructure(const std::string &seriesCode);
    // Compute the global range (min and max mean) from the build array.
    std::string computeRange() const;
    // Return a space-separated list of country names from the build array that satisfy the condition.
    std::string computeFind(double mean, const std::string &op) const;
    // Remove an entry (by country name) from the build array.
    bool deleteFromBuild(const std::string &countryName);
    // Compute the limits (lowest or highest) from the build array.
    std::string computeLimits(const std::string &condition) const;
    // Find a country (by name) in the hash table.
    std::string findCountry(const std::string &countryName) const;
    // Free any previously built array.
    void deleteBuildEntries();

    // --- Modified LOAD: Reads a CSV file and uses hashing to store countries.
    bool loadFromFile(const std::string &filename);

public:
    CountryData();
    ~CountryData();

    // Project 3 Commands (maintained, implemented via hashing and linear scans) 
    bool load(const std::string &filename);              // LOAD
    bool buildCommand(const std::string &seriesCode);      // BUILD
    std::string rangeCommand(const std::string &seriesCode); // RANGE
    std::string listCommand(const std::string &countryName); // LIST
    std::string findCommand(double mean, const std::string &op); // FIND
    bool deleteCommand(const std::string &countryName);    // DELETE (by country name)
    std::string limitsCommand(const std::string &condition); // LIMITS

    // Project 4 Commands
    bool insertCommand(const std::string &code, const std::string &filename); // INSERT
    std::pair<int,int> lookupCommand(const std::string &code) const;          // LOOKUP
    bool removeCommand(const std::string &code);                              // REMOVE
};

#endif
