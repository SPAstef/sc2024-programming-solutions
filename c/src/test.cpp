#include <compare>
#include <print>
#include <random>
#include <vector>

class Date
{
private:
    int y;
    int m;
    int d;

public:
    Date() = default;
    Date(const Date &) = default;
    Date(Date &&) = default;


    Date(int d, int m, int y) : d{d}, m{m}, y{y} {}

    Date &operator=(const Date &) = default;
    Date &operator=(Date &&) = default;

    auto operator<=>(const Date &) const noexcept = default;

    int day() const noexcept { return d; }
    int month() const noexcept { return m; }
    int year() const noexcept { return y; }
};

int main()
{
    std::vector<Date> dates(10);
    std::random_device hw_rng;
    std::mt19937_64 prng{hw_rng()};
    std::uniform_int_distribution random_d{1, 30};
    std::uniform_int_distribution random_m{1, 12};
    std::uniform_int_distribution random_y{1900, 1999};

    std::ranges::generate(dates,
                          [&] {
                              return Date{random_d(prng), random_m(prng), random_y(prng)};
                          });

    std::println("Initial dates:");
    for (auto &&date : dates)
        std::println("{:02}/{:02}/{:04}", date.day(), date.month(), date.year());

    std::ranges::sort(dates);

    std::println("Sorted dates:");
    for (auto &&date : dates)
        std::println("{:02}/{:02}/{:04}", date.day(), date.month(), date.year());

    return 0;
}
