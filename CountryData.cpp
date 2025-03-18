#include "CountryData.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

// SERIES IMPLEMENTATION
CountryData::Series::Series(const std::string &name, const std::string &code)
    : seriesName(name), seriesCode(code), years(nullptr), values(nullptr),
      numEntries(0), maxEntries(10), next(nullptr)
{
    // Allocate initial arrays
    years = new int[maxEntries];
    values = new double[maxEntries];
    for (int i = 0; i < maxEntries; i++) {
        years[i] = 0;
        values[i] = 0.0;
    }
}

CountryData::Series::~Series() {
    delete[] years;
    delete[] values;
    years = nullptr;
    values = nullptr;
}

void CountryData::Series::resize() {
    int newMax = maxEntries * 2;
    int *newYears = new int[newMax];
    double *newValues = new double[newMax];
    for (int i = 0; i < newMax; i++) {
        newYears[i] = 0;
        newValues[i] = 0.0;
    }
    for (int i = 0; i < numEntries; i++) {
        newYears[i] = years[i];
        newValues[i] = values[i];
    }
    delete[] years;
    delete[] values;
    years = newYears;
    values = newValues;
    maxEntries = newMax;
}

// COUNTRYNODE IMPLEMENTATION
CountryData::CountryNode::CountryNode(const std::string &name, const std::string &code)
    : countryName(name), countryCode(code), timeSeries(nullptr)
{
}

CountryData::CountryNode::~CountryNode() {
    Series *current = timeSeries;
    while (current != nullptr) {
        Series *next = current->next;
        delete current;
        current = next;
    }
    timeSeries = nullptr;
}

// COUNTRYDATA IMPLEMENTATION 
CountryData::CountryData() 
    : countryCount(0), buildEntries(nullptr), buildSize(0), buildCapacity(0), lastBuiltSeries("")
{
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        countryArray[i] = nullptr;
        slotStatus[i] = STATUS_EMPTY;
    }
}


CountryData::~CountryData() {
    // Clean up hash table
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        if (countryArray[i] != nullptr) {
            delete countryArray[i];
            countryArray[i] = nullptr;
        }
        slotStatus[i] = STATUS_EMPTY;
    }
    countryCount = 0;
    deleteBuildEntries();
}

// Hashing Helper Methods
unsigned int CountryData::codeToInteger(const std::string &code) const {
    unsigned int val = 0;
    for (int i = 0; i < 3; i++) {
        unsigned int letterVal = (unsigned int)(code[i] - 'A');
        val = val * 26 + letterVal;
    }
    return val;
}

unsigned int CountryData::h1(unsigned int W) const {
    return W % MAX_COUNTRIES;
}

unsigned int CountryData::h2(unsigned int W) const {
    unsigned int temp = W / MAX_COUNTRIES;
    unsigned int step = temp % MAX_COUNTRIES;
    if (step % 2 == 0)
        step += 1;
    return step;
}

bool CountryData::hashInsert(CountryNode *newCountry) {
    std::string code = newCountry->countryCode;
    unsigned int W = codeToInteger(code);
    unsigned int index = h1(W);
    unsigned int step = h2(W);
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        unsigned int pos = (index + i * step) % MAX_COUNTRIES;
        if (slotStatus[pos] == STATUS_OCCUPIED) {
            if (countryArray[pos]->countryCode == code)
                return false;
        } else if (slotStatus[pos] == STATUS_EMPTY || slotStatus[pos] == STATUS_PREV_OCCUPIED) {
            countryArray[pos] = newCountry;
            slotStatus[pos] = STATUS_OCCUPIED;
            countryCount++;
            return true;
        }
    }
    return false;
}

std::pair<int,int> CountryData::hashSearch(const std::string &code) const {
    unsigned int W = codeToInteger(code);
    unsigned int index = h1(W);
    unsigned int step = h2(W);
    int probes = 0;
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        unsigned int pos = (index + i * step) % MAX_COUNTRIES;
        probes++;
        if (slotStatus[pos] == STATUS_OCCUPIED && countryArray[pos] != nullptr) {
            if (countryArray[pos]->countryCode == code)
                return std::make_pair((int)pos, probes);
        } else if (slotStatus[pos] == STATUS_EMPTY) {
            return std::make_pair(-1, probes);
        }
    }
    return std::make_pair(-1, probes);
}

bool CountryData::hashRemove(const std::string &code) {
    unsigned int W = codeToInteger(code);
    unsigned int index = h1(W);
    unsigned int step = h2(W);
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        unsigned int pos = (index + i * step) % MAX_COUNTRIES;
        if (slotStatus[pos] == STATUS_OCCUPIED && countryArray[pos] != nullptr) {
            if (countryArray[pos]->countryCode == code) {
                delete countryArray[pos];
                countryArray[pos] = nullptr;
                slotStatus[pos] = STATUS_PREV_OCCUPIED;
                countryCount--;
                return true;
            }
        } else if (slotStatus[pos] == STATUS_EMPTY) {
            return false;
        }
    }
    return false;
}

// Other commands
void CountryData::deleteBuildEntries() {
    if (buildEntries != nullptr) {
        for (int i = 0; i < buildSize; i++) {
            delete buildEntries[i];
        }
        delete[] buildEntries;
        buildEntries = nullptr;
        buildSize = 0;
        buildCapacity = 0;
    }
}

bool CountryData::buildStructure(const std::string &seriesCode) {
    deleteBuildEntries();
    int capacity = countryCount;
    buildCapacity = (capacity > 0) ? capacity : 1;
    buildEntries = new Entry*[buildCapacity];
    for (int i = 0; i < buildCapacity; i++)
        buildEntries[i] = nullptr;
    buildSize = 0;
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        if (slotStatus[i] == STATUS_OCCUPIED && countryArray[i] != nullptr) {
            CountryNode *c = countryArray[i];
            Series *s = c->timeSeries;
            bool found = false;
            double sum = 0.0;
            int validCount = 0;
            while (s != nullptr) {
                if (s->seriesCode == seriesCode) {
                    found = true;
                    for (int j = 0; j < s->numEntries; j++) {
                        if (s->values[j] != -1) {
                            sum += s->values[j];
                            validCount++;
                        }
                    }
                    break;
                }
                s = s->next;
            }
            if (found) {
                double mean = (validCount > 0) ? (sum / validCount) : 0.0;
                Entry *e = new Entry();
                e->countryName = c->countryName;
                e->mean = mean;
                buildEntries[buildSize++] = e;
            }
        }
    }
    return (buildSize > 0);
}

bool CountryData::buildCommand(const std::string &seriesCode) {
    bool built = buildStructure(seriesCode);
    // Record the series code used for BUILD.
    lastBuiltSeries = seriesCode;
    return built; 
}

std::string CountryData::computeRange() const {
    if (buildSize == 0)
        return "failure";
    double globalMin = buildEntries[0]->mean;
    double globalMax = buildEntries[0]->mean;
    for (int i = 1; i < buildSize; i++) {
        double m = buildEntries[i]->mean;
        if (m < globalMin) globalMin = m;
        if (m > globalMax) globalMax = m;
    }
    std::ostringstream oss;
    oss << globalMin << " " << globalMax;
    return oss.str();
}

std::string CountryData::rangeCommand(const std::string &seriesCode) {
    // assume BUILD was called before.
    return computeRange();
}

std::string CountryData::computeFind(double mean, const std::string &op) const {
    if (buildSize == 0)
        return "failure";
    std::string result;
    for (int i = 0; i < buildSize; i++) {
        double m = buildEntries[i]->mean;
        bool match = false;
        if (op == "less")
            match = (m < mean);
        else if (op == "greater")
            match = (m > mean);
        else if (op == "equal")
            match = (fabs(m - mean) < 0.001);
        if (match)
            result += buildEntries[i]->countryName + " ";
    }
    if (!result.empty() && result.back() == ' ')
        result.pop_back();
    return result.empty() ? "failure" : result;
}

std::string CountryData::findCommand(double mean, const std::string &op) {
    return computeFind(mean, op);
}

bool CountryData::deleteFromBuild(const std::string &countryName) {
    bool found = false;
    for (int i = 0; i < buildSize; ) {
        if (buildEntries[i]->countryName == countryName) {
            delete buildEntries[i];
            for (int j = i; j < buildSize - 1; j++) {
                buildEntries[j] = buildEntries[j+1];
            }
            buildSize--;
            found = true;
        } else {
            i++;
        }
    }
    return found;
}

bool CountryData::deleteCommand(const std::string &countryName) {
    // Search the hash table for the country
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        if (slotStatus[i] == STATUS_OCCUPIED && countryArray[i] != nullptr) {
            if (countryArray[i]->countryName == countryName) {
                // Found in the hash table => remove from hash table
                std::string code = countryArray[i]->countryCode;
                bool removed = hashRemove(code);  // Must be true if we found it
                // Also remove from build array
                deleteFromBuild(countryName);
                return removed; // Return success (true) if found in hash
            }
        }
    }
    
    // If we get here, the country was not in the hash table => return failure
    return false;
}

std::string CountryData::computeLimits(const std::string &condition) const {
    if (buildSize == 0)
        return "failure";
    double extreme = buildEntries[0]->mean;
    for (int i = 1; i < buildSize; i++) {
        double m = buildEntries[i]->mean;
        if (condition == "lowest" && m < extreme)
            extreme = m;
        else if (condition == "highest" && m > extreme)
            extreme = m;
    }
    std::string result;
    for (int i = 0; i < buildSize; i++) {
        if (fabs(buildEntries[i]->mean - extreme) < 0.001)
            result += buildEntries[i]->countryName + " ";
    }
    if (!result.empty() && result.back() == ' ')
        result.pop_back();
    return result.empty() ? "failure" : result;
}

std::string CountryData::limitsCommand(const std::string &condition) {
    return computeLimits(condition);
}

std::string CountryData::findCountry(const std::string &countryName) const {
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        if (slotStatus[i] == STATUS_OCCUPIED && countryArray[i] != nullptr) {
            if (countryArray[i]->countryName == countryName) {
                std::ostringstream oss;
                oss << countryArray[i]->countryName << " " << countryArray[i]->countryCode;
                for (Series *s = countryArray[i]->timeSeries; s != nullptr; s = s->next)
                    oss << " " << s->seriesName;
                return oss.str();
            }
        }
    }
    return "failure";
}

std::string CountryData::listCommand(const std::string &countryName) {
    return findCountry(countryName);
}

// P4 Commands
bool CountryData::insertCommand(const std::string &code, const std::string &filename) {
    std::pair<int,int> sr = hashSearch(code);
    if (sr.first != -1)
        return false; // Already in table.
    std::ifstream fin(filename);
    if (!fin.is_open())
        return false;
    CountryNode *newC = nullptr;
    bool foundLine = false;
    std::string line;
    while (std::getline(fin, line)) {
        std::stringstream ss(line);
        std::string cName, cCode, sName, sCode;
        std::getline(ss, cName, ',');
        std::getline(ss, cCode, ',');
        std::getline(ss, sName, ',');
        std::getline(ss, sCode, ',');
        if (cCode == code) {
            // If this is the first matching line, create the country node.
            if (!foundLine) {
                newC = new CountryNode(cName, cCode);
                foundLine = true;
            }
            // Create a new series for this line.
            Series *newSeries = new Series(sName, sCode);
            int year = 1960;
            std::string val;
            while (std::getline(ss, val, ',')) {
                double numValue = (val.empty() || val == "-1") ? -1 : std::stod(val);
                if (newSeries->numEntries >= newSeries->maxEntries)
                    newSeries->resize();
                newSeries->years[newSeries->numEntries] = year;
                newSeries->values[newSeries->numEntries] = numValue;
                newSeries->numEntries++;
                year++;
            }
            // Append newSeries to the country's timeSeries linked list.
            if (newC->timeSeries == nullptr) {
                newC->timeSeries = newSeries;
            } else {
                Series *cursor = newC->timeSeries;
                while (cursor->next != nullptr)
                    cursor = cursor->next;
                cursor->next = newSeries;
            }
        }
    }

    fin.close();
    if (!newC)
        return false;
    bool ok = hashInsert(newC);
    if (!ok) {
        delete newC;
        return false;
    }
    // Now, update the build structure if a BUILD was previously done.
    if (!lastBuiltSeries.empty()) {
        // Check if the new country has the lastBuiltSeries data.
        Series *s = newC->timeSeries;
        bool found = false;
        double sum = 0.0;
        int validCount = 0;
        while (s != nullptr) {
            if (s->seriesCode == lastBuiltSeries) {
                found = true;
                for (int j = 0; j < s->numEntries; j++) {
                    if (s->values[j] != -1) {
                        sum += s->values[j];
                        validCount++;
                    }
                }
                break;
            }
            s = s->next;
        }
        if (found) {
            double mean = (validCount > 0) ? (sum / validCount) : 0.0;
            // If buildEntries is not allocated, allocate it.
            if (buildEntries == nullptr) {
                buildCapacity = 1;
                buildEntries = new Entry*[buildCapacity];
                buildSize = 0;
            } else if (buildSize >= buildCapacity) {
                // Reallocate: double the capacity.
                int newCapacity = (buildCapacity == 0 ? 1 : buildCapacity * 2);
                Entry **newEntries = new Entry*[newCapacity];
                for (int i = 0; i < buildSize; i++) {
                    newEntries[i] = buildEntries[i];
                }
                delete[] buildEntries;
                buildEntries = newEntries;
                buildCapacity = newCapacity;
            }
            Entry *e = new Entry();
            e->countryName = newC->countryName;
            e->mean = mean;
            buildEntries[buildSize++] = e;
        }
    }
    return true;
}

std::pair<int,int> CountryData::lookupCommand(const std::string &code) const {
    return hashSearch(code);
}

bool CountryData::removeCommand(const std::string &code) {
    return hashRemove(code);
}

// LOAD Command (Using Hashing)
bool CountryData::loadFromFile(const std::string &filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        return false;
    }
    // Clear existing data
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        if (countryArray[i] != nullptr) {
            delete countryArray[i];
            countryArray[i] = nullptr;
        }
        slotStatus[i] = STATUS_EMPTY;
    }
    countryCount = 0;
    deleteBuildEntries();

    std::string line;
    while (std::getline(fin, line)) {
        if (countryCount >= MAX_COUNTRIES)
            break;
        std::stringstream ss(line);
        std::string cName, cCode, sName, sCode;
        std::getline(ss, cName, ',');
        std::getline(ss, cCode, ',');
        std::getline(ss, sName, ',');
        std::getline(ss, sCode, ',');

        std::pair<int,int> sr = hashSearch(cCode);
        CountryNode *cn = nullptr;
        if (sr.first == -1) {
            cn = new CountryNode(cName, cCode);
            if (!hashInsert(cn)) {
                delete cn;
                break;
            }
        } else {
            cn = countryArray[sr.first];
        }

        Series *newS = new Series(sName, sCode);
        int year = 1960;
        std::string val;
        while (std::getline(ss, val, ',')) {
            double numValue = (val.empty() || val == "-1") ? -1 : std::stod(val);
            if (newS->numEntries >= newS->maxEntries)
                newS->resize();
            newS->years[newS->numEntries] = year;
            newS->values[newS->numEntries] = numValue;
            newS->numEntries++;
            year++;
        }
        if (cn->timeSeries == nullptr)
            cn->timeSeries = newS;
        else {
            Series *cursor = cn->timeSeries;
            while (cursor->next != nullptr)
                cursor = cursor->next;
            cursor->next = newS;
        }
    }
    fin.close();
    return true;
}

bool CountryData::load(const std::string &filename) {
    return loadFromFile(filename);
}
