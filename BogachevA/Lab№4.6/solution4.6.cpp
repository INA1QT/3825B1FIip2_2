#include <iostream>
#include <stdexcept>
#include <fstream>
#include <string>
#include <algorithm> 
using std::cin;
using std::cout;
using std::string;
using std::endl;
using std::getline;
enum class Month {
    January = 1, February, March, April, May,
    June, July, August, September, October, November, December
};
inline int monthToInt(Month m) {
    return static_cast<int>(m);
}
inline Month intToMonth(int m) {
    return static_cast<Month>(m);
}
class Date {
private:
    int day;
    Month month;
    int year;
    static bool isLeapYear(int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }
    static int daysInMonth(Month m, int y) {
        int monthInt = monthToInt(m);
        switch (monthInt) {
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return isLeapYear(y) ? 29 : 28;
        default: return 31;
        }
    }
    bool isCorrectData() const {
        if (year < 1895 || day < 1) {
            return false;
        }
        int maxDays = daysInMonth(month, year);
        return day <= maxDays;
    }
public:
    explicit Date(int day_ = 28, Month month_ = Month::December, int year_ = 1895) : day(day_), month(month_), year(year_) {
        if (!isCorrectData()) {
            throw std::invalid_argument("incorrect date");
        }
    }
    Month getMonth() const {
        return month;
    }
    int getDay() const {
        return day;
    }
    int getYear() const {
        return year;
    }
};
class Movie {
private:
    string title;
    string director;
    string screenwriter;
    string composer;
    Date date;
    unsigned int boxOffice;
public:
    explicit Movie(string t = "Unknown", string d = "Unknown", string s = "Unknown", string c = "Unknown", Date date_ = Date(), unsigned int boxOffice_ = 0) :
        title(t), director(d), screenwriter(s), composer(c), date(date_), boxOffice(boxOffice_) {
    }
    ~Movie() = default;
    string getTitle() const {
        return title;
    }
    string getDirector() const {
        return director;
    }
    string getScreenwriter() const {
        return screenwriter;
    }
    string getComposer() const {
        return composer;
    }
    Date getDate() const {
        return date;
    }
    unsigned int getBoxOffice() const {
        return boxOffice;
    }
    int getYear() const {
        return date.getYear();
    }
    void setTitle(const string& t) {
        title = t;
    }
    void setDirector(const string& d) {
        director = d;
    }
    void setScreenwriter(const string& s) {
        screenwriter = s;
    }
    void setComposer(const string& c) {
        composer = c;
    }
    void setDate(const Date& d) {
        date = d;
    }
    void setBoxOffice(unsigned int b) {
        boxOffice = b;
    }
    void print() const {
        cout << "Title is: " << title << endl
            << "Directed by: " << director << endl
            << "Screenwriter: " << screenwriter << endl
            << "Composed by: " << composer << endl
            << "Date:" << date.getDay() << "." << monthToInt(date.getMonth()) << "." << date.getYear() << endl
            << "Box office: " << boxOffice << " rubles" << endl;
    }
};
Movie inputMovie() {
    string title, director, screenwriter, composer;
    int day, monthInt, year;
    unsigned int boxOffice;
    cout << "Enter the title: ";
    getline(cin, title);
    cout << "Enter the name of the director: ";
    getline(cin, director);
    cout << "Enter the name of the screenwriter: ";
    getline(cin, screenwriter);
    cout << "Enter the name of the composer: ";
    getline(cin, composer);
    cout << "Enter the day 1-31(30 or 28-29): ";
    cin >> day;
    cout << "Enter the month 1-12: ";
    cin >> monthInt;
    cout << "Enter the year: ";
    cin >> year;
    cout << "Enter the box office in rubles: ";
    cin >> boxOffice;
    cin.ignore();
    Date date(day, intToMonth(monthInt), year);
    return Movie(title, director, screenwriter, composer, date, boxOffice);
}
class filmLibrary {
private:
    Movie* movies;
    size_t size;
    size_t capacity;
    void expandCapacity() {
        if (size < capacity) {
            return;
        }
        size_t newCap = (capacity == 0) ? 1 : capacity * 2;
        Movie* newMovies = nullptr;
        try {
            newMovies = new Movie[newCap];
        }
        catch (...) {
            throw std::runtime_error("Memory allocation failed");
        }
        try {
            for (size_t i = 0; i < size; ++i) {
                newMovies[i] = movies[i];
            }
        }
        catch (...) {
            delete[] newMovies;
            throw;
        }
        delete[] movies;
        movies = newMovies;
        capacity = newCap;
    }
    size_t findInsertPosition(const Movie& m) const {
        size_t left = 0, right = size;
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            const Movie& cur = movies[mid];
            if (m.getTitle() < cur.getTitle() ||
                (m.getTitle() == cur.getTitle() && m.getYear() < cur.getYear())) {
                right = mid;
            }
            else {
                left = mid + 1;
            }
        }
        return left;
    }
    bool exists(const string& title, int year) const {
        for (size_t i = 0; i < size; ++i) {
            if (movies[i].getTitle() == title && movies[i].getYear() == year)
                return true;
        }
        return false;
    }
    void merge(int left, int mid, int right, Movie* temp) {
        int i = left;
        int j = mid + 1;
        int k = left;
        while (i <= mid && j <= right) {
            string titleI = movies[i].getTitle();
            string titleJ = movies[j].getTitle();
            if (titleI < titleJ || (titleI == titleJ && movies[i].getYear() < movies[j].getYear())) {
                temp[k++] = movies[i++];
            }
            else {
                temp[k++] = movies[j++];
            }
        }
        while (i <= mid) {
            temp[k++] = movies[i++];
        }
        while (j <= right) {
            temp[k++] = movies[j++];
        }
        for (i = left; i <= right; ++i) {
            movies[i] = temp[i];
        }
    }
    void mergeSort(int left, int right, Movie* temp) {
        if (left >= right) {
            return;
        }
        int mid = left + (right - left) / 2;
        mergeSort(left, mid, temp);
        mergeSort(mid + 1, right, temp);
        merge(left, mid, right, temp);
    }
    void sortMovies() {
        if (size <= 1) {
            return;
        }
        Movie* temp = nullptr;
        try {
            temp = new Movie[size];
        }
        catch (...) {
            throw std::runtime_error("Memory allocation failed during the sorting");
        }
        mergeSort(0, size - 1, temp);
        delete[] temp;
    }
    int findIndex(const string& title, int year) const {
        for (size_t i = 0; i < size; ++i) {
            if (movies[i].getTitle() == title && movies[i].getYear() == year)
                return (int)i;
        }
        return -1;
    }
public:
    explicit filmLibrary() : movies(nullptr), size(0), capacity(0) {}
    ~filmLibrary() {
        delete[] movies;
    }
    filmLibrary(const filmLibrary& other) : movies(nullptr), size(other.size), capacity(other.capacity) {
        try {
            movies = new Movie[capacity];
        }
        catch (...) {
            throw std::runtime_error("Memory size is not enough");
        }
        try {
            for (size_t i = 0; i < size; ++i) {
                movies[i] = other.movies[i];
            }
        }
        catch (...) {
            delete[] movies;
            throw;
        }
    }
    filmLibrary& operator=(const filmLibrary& other) {
        if (this != &other) {
            filmLibrary temp(other);
            std::swap(movies, temp.movies);
            std::swap(size, temp.size);
            std::swap(capacity, temp.capacity);
        }
        return *this;
    }
    void addMovie(const Movie& m) {
        if (exists(m.getTitle(), m.getYear())) {
            throw std::runtime_error("Movie with this title and year already exists");
        }
        expandCapacity();
        size_t pos = findInsertPosition(m);
        for (size_t i = size; i > pos; --i) {
            movies[i] = movies[i - 1];
        }
        movies[pos] = m;
        ++size;
    }
    bool editMovie(const string& title, int year, const Movie& newData) {
        int idx = findIndex(title, year);
        if (idx == -1) {
            return false;
        }
        if (newData.getTitle() != title || newData.getYear() != year) {
            if (exists(newData.getTitle(), newData.getYear())) {
                return false;
            }
        }
        for (size_t i = idx; i < size - 1; ++i) {
            movies[i] = movies[i + 1];
        }
        --size;
        expandCapacity();
        size_t pos = findInsertPosition(newData);
        for (size_t i = size; i > pos; --i) {
            movies[i] = movies[i - 1];
        }
        movies[pos] = newData;
        ++size;
        return true;
    }
    const Movie* findMovie(const string& title, int year) const {
        int idx = findIndex(title, year);
        return (idx != -1) ? &movies[idx] : nullptr;
    }
    Movie* getMoviesByDirector(const string& director, size_t& outSize) const {
        outSize = 0;
        for (size_t i = 0; i < size; ++i) {
            if (movies[i].getDirector() == director) {
                ++outSize;
            }
        }
        if (outSize == 0) {
            return nullptr;
        }
        Movie* result = nullptr;
        try {
            result = new Movie[outSize];
        }
        catch (...) {
            throw std::runtime_error("Memory allocation failed in getMoviesByDirector");
        }
        size_t pos = 0;
        for (size_t i = 0; i < size; ++i) {
            if (movies[i].getDirector() == director) {
                result[pos++] = movies[i];
            }
        }
        return result;
    }
    Movie* getMoviesByYear(int year, size_t& outSize) const {
        outSize = 0;
        for (size_t i = 0; i < size; ++i) {
            if (movies[i].getYear() == year) {
                ++outSize;
            }
        }
        if (outSize == 0) {
            return nullptr;
        }
        Movie* result = nullptr;
        try {
            result = new Movie[outSize];
        }
        catch (...) {
            throw std::runtime_error("Memory allocation failed in getMoviesByYear");
        }
        size_t pos = 0;
        for (size_t i = 0; i < size; ++i) {
            if (movies[i].getYear() == year) {
                result[pos++] = movies[i];
            }
        }
        return result;
    }
    Movie* getTopMovies(size_t n, size_t& outSize) const {
        if (size == 0) {
            outSize = 0;
            return nullptr;
        }
        outSize = (n < size) ? n : size;
        Movie* allCopy = nullptr;
        try {
            allCopy = new Movie[size];
        }
        catch (...) {
            throw std::runtime_error("Memory allocation failed in getTopMovies (allCopy)");
        }
        for (size_t i = 0; i < size; ++i) {
            allCopy[i] = movies[i];
        }
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = i + 1; j < size; ++j) {
                if (allCopy[i].getBoxOffice() < allCopy[j].getBoxOffice()) {
                    std::swap(allCopy[i], allCopy[j]);
                }
            }
        }
        Movie* result = nullptr;
        try {
            result = new Movie[outSize];
        }
        catch (...) {
            delete[] allCopy;
            throw std::runtime_error("Memory allocation failed in getTopMovies (result)");
        }
        for (size_t i = 0; i < outSize; ++i) {
            result[i] = allCopy[i];
        }
        delete[] allCopy;
        return result;
    }
    Movie* getTopMoviesByYear(int year, size_t n, size_t& outSize) const {
        size_t yearCount = 0;
        for (size_t i = 0; i < size; ++i) {
            if (movies[i].getYear() == year) {
                ++yearCount;
            }
        }
        if (yearCount == 0) {
            outSize = 0;
            return nullptr;
        }
        Movie* yearMovies = nullptr;
        try {
            yearMovies = new Movie[yearCount];
        }
        catch (...) {
            throw std::runtime_error("Memory allocation failed in getTopMoviesByYear (yearMovies)");
        }
        size_t pos = 0;
        for (size_t i = 0; i < size; ++i) {
            if (movies[i].getYear() == year) {
                yearMovies[pos++] = movies[i];
            }
        }
        for (size_t i = 0; i < yearCount; ++i) {
            for (size_t j = i + 1; j < yearCount; ++j) {
                if (yearMovies[i].getBoxOffice() < yearMovies[j].getBoxOffice()) {
                    std::swap(yearMovies[i], yearMovies[j]);
                }
            }
        }
        outSize = (n < yearCount) ? n : yearCount;
        if (outSize == 0) {
            delete[] yearMovies;
            return nullptr;
        }
        Movie* result = nullptr;
        try {
            result = new Movie[outSize];
        }
        catch (...) {
            delete[] yearMovies;
            throw std::runtime_error("Memory allocation failed in getTopMoviesByYear (result)");
        }
        for (size_t i = 0; i < outSize; ++i) {
            result[i] = yearMovies[i];
        }
        delete[] yearMovies;
        return result;
    }
    size_t getSize() const {
        return size;
    }
    bool removeMovie(const string& title, int year) {
        int idx = findIndex(title, year);
        if (idx == -1) {
            return false;
        }
        for (size_t i = idx; i < size - 1; ++i) {
            movies[i] = movies[i + 1];
        }
        --size;
        return true;
    }
    void saveToFile(const string& filename) const {
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("No such file");
        }
        out << size << endl;
        for (size_t i = 0; i < size; ++i) {
            out << movies[i].getTitle() << endl;
            out << movies[i].getDirector() << endl;
            out << movies[i].getScreenwriter() << endl;
            out << movies[i].getComposer() << endl;
            out << movies[i].getDate().getDay() << ' ' << monthToInt(movies[i].getDate().getMonth()) << ' ' << movies[i].getDate().getYear() << endl;
            out << movies[i].getBoxOffice() << endl;
        }
    }
    void loadFromFile(const string& filename) {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("No such file");
        }
        size_t newSize;
        in >> newSize;
        in.ignore();
        delete[] movies;
        movies = nullptr;
        size = 0;
        capacity = newSize;
        try {
            movies = new Movie[capacity];
        }
        catch (...) {
            throw std::runtime_error("not enough memory");
        }
        for (size_t i = 0; i < newSize; ++i) {
            string title, director, screenwriter, composer;
            int day, monthInt, year;
            unsigned int boxOffice;
            getline(in, title);
            getline(in, director);
            getline(in, screenwriter);
            getline(in, composer);
            in >> day >> monthInt >> year;
            in.ignore();
            in >> boxOffice;
            in.ignore();
            try {
                movies[size++] = Movie(title, director, screenwriter, composer,
                    Date(day, intToMonth(monthInt), year), boxOffice);
            }
            catch (...) {
                delete[] movies;
                throw std::runtime_error("fatal error");
            }
        }
        sortMovies();
    }
    void printAll() const {
        if (size == 0) {
            cout << "The film library is empty" << endl;
            return;
        }
        for (size_t i = 0; i < size; ++i) {
            movies[i].print();
        }
    }
};
void printAndDeleteArray(Movie* arr, size_t count) {
    if (arr == nullptr || count == 0) {
        cout << "No movies" << endl;
        return;
    }
    for (size_t i = 0; i < count; ++i) {
        arr[i].print();
    }
    delete[] arr;
}
int main() {
    filmLibrary ZolotayaMalina;
    int choice;
    do {
        cout << "Menu"
            << "1.Add movie" << endl
            << "2. Change movie data" << endl
            << "3. Find movie by year and title" << endl
            << "4. Find all movies by the chosen director" << endl
            << "5.Finl all movies by year" << endl
            << "6.Find top N movies by box office" << endl
            << "7. Find top N movies by box office in chosen year" << endl
            << "8.Find the number of movies in the library" << endl
            << "9.Delete movie from the library" << endl
            << "10.Save the library to the file"
            << "11.Load the library from file" << endl
            << "12.Print the library" << endl
            << "0.Exit" << endl
            << "Your choice is: ";
        cin >> choice;
        cin.ignore();
        try {
            switch (choice) {
            case 1:
                ZolotayaMalina.addMovie(inputMovie());
                cout << "Movie has been added to the library" << endl;
                break;
            case 2: {
                string title;
                int year;
                cout << "Enter the title of movie tha you want to change: ";
                getline(cin, title);
                cout << "Enter the year: ";
                cin >> year;
                cin.ignore();
                const Movie* old = ZolotayaMalina.findMovie(title, year);
                if (!old) {
                    cout << "No such film in the library";
                    break;
                }
                cout << "Enter new data" << endl;
                Movie newData = inputMovie();
                if (ZolotayaMalina.editMovie(title, year, newData))
                    cout << "Data has been succesfuly changed" << endl;
                else
                    cout << "Error" << endl;
                break;
            }
            case 3: {
                string title;
                int year;
                cout << "Enter the title: ";
                getline(cin, title);
                cout << "Enter the year: ";
                cin >> year;
                cin.ignore();
                const Movie* found = ZolotayaMalina.findMovie(title, year);
                if (found) {
                    found->print();
                }
                else {
                    cout << "Movie has not been found" << endl;
                }
                break;
            }
            case 4: {
                string director;
                cout << "Enter the name of the director: ";
                getline(cin, director);
                size_t temp;
                Movie* arr = ZolotayaMalina.getMoviesByDirector(director, temp);
                printAndDeleteArray(arr, temp);
                break;
            }
            case 5: {
                int year;
                cout << "Enter the year: ";
                cin >> year;
                cin.ignore();
                size_t temp;
                Movie* arr = ZolotayaMalina.getMoviesByYear(year, temp);
                printAndDeleteArray(arr, temp);
                break;
            }
            case 6: {
                size_t n;
                cout << "Enter N: ";
                cin >> n;
                cin.ignore();
                size_t temp;
                Movie* top = ZolotayaMalina.getTopMovies(n, temp);
                printAndDeleteArray(top, temp);
                break;
            }
            case 7: {
                int year;
                size_t n;
                cout << "Enter the year: ";
                cin >> year;
                cout << "Enter N: ";
                cin >> n;
                cin.ignore();
                size_t temp;
                Movie* top = ZolotayaMalina.getTopMoviesByYear(year, n, temp);
                printAndDeleteArray(top, temp);
                break;
            }
            case 8:
                cout << ZolotayaMalina.getSize() << " movies" << endl;
                break;
            case 9: {
                string title;
                int year;
                cout << "Enter the title of the movie which you want to be deleted: ";
                getline(cin, title);
                cout << "Enter the year: ";
                cin >> year;
                cin.ignore();
                if (ZolotayaMalina.removeMovie(title, year))
                    cout << "Movie has been succesfuly deleted" << endl;
                else
                    cout << "No such film in the library" << endl;
                break;
            }
            case 10: {
                string fname;
                cout << "Enter the filename: ";
                getline(cin, fname);
                ZolotayaMalina.saveToFile(fname);
                cout << "Saved!" << endl;
                break;
            }
            case 11: {
                string fname;
                cout << "Enter the filename: ";
                getline(cin, fname);
                ZolotayaMalina.loadFromFile(fname);
                cout << "Loaded!" << endl;
                break;
            }
            case 12:
                ZolotayaMalina.printAll();
                break;
            case 0:
                cout << "Goodbye!";
                break;
            default:
                cout << "Incorrect choice, try again!";
            }
        }
        catch (const std::exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    } while (choice != 0);
    return 0;
}