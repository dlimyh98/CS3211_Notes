#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

struct Token {
    std::string secret;
};

class TokenPromise {
    Token token;
    std::atomic<bool> isValid;

public:
    TokenPromise() : token{}, isValid{ false } {}

    // May only be called once
    void publishToken(Token t) {
        assert(!isValid);
        token = t;              // potential data race

        // INCORRECT: Use a relaxed store (relaxed has no Acquire-Release semantics)
        isValid.store(true, std::memory_order_release);
    }

    // store to token is sequenced before isValid.store (due to Program Order)
    // load from token is sequence after isValid.load (due to Program Order)
    // isValid.store() synchronizes with isValid.load() (due to Acquire-Release semantics)
    
    // No limits on who or how many times this can be called
    std::optional<Token> get_token() {
        // INCORRECT: Use a relaxed load (relaxed has no Acquire-Release semantics)
        if (isValid.load(std::memory_order_acquire)) {
            return token;      // potential data race
        }
        else {
            return std::nullopt;
        }
    }
};



TokenPromise luminus_password{};

int main() {
    std::thread get_password{ []() {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      luminus_password.publishToken(Token{"abcdef"});
    } };

    std::thread use_password{ []() {
      while (true) {
        std::optional<Token> t = luminus_password.get_token();
        if (t) {
          std::cout << t->secret << std::endl;
          break;
        }
      }
    } };

    get_password.join();
    use_password.join();

    return 0;
}