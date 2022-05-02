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
    // INCORRECT: use bool isValid; 
    std::atomic<bool> isValid;

public:
    // We start out with no token
    TokenPromise() : token{}, isValid{ false } {}

    // May only be called once
    void publishToken(Token t) {
        assert(!isValid);
        token = t;
        isValid = true;     // equivalent to isValid.store(true, std::memory_order_seq_cst)
        // a STORE operation with memory_order_seq_cst performs a memeory_release operation
        // memory_order_release ensures that no READS OR WRITES can be reordered AFTER this store
    }

    // possible data race between STORE to token in publishToken() and LOAD from token in get_token()
    // this is solved here as isValid = true SYNCS with if(isValid). i.e. Release-Acquire semantics!
    // note that THERE IS NO ENFORCED ORDERING between LOADS AND STORE in general!

    // No limits on who or how many times this can be called
    std::optional<Token> get_token() {
        if (isValid) {    // equivalent to isValid.load(std::memory_order_seq_cst)
        // a LOAD operation with memory_order_seq_cst performs a memeory_acquire operation
        // memory_order_acquire ensures that no READS OR WRITES can be reordered BEFORE this load
            return token;
        }
        else {
            return std::nullopt;
        }
    }
};



TokenPromise luminus_password{};

int main() {

    // one thread calls publishToken(), to WRITE to token
    std::thread get_password{ []() {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      luminus_password.publishToken(Token{"abcdef"});
    } };


    // another thread calls get_token(), to READ from token
    // from the perspective of a SINGLE-THREADED program, while loop only ever READS from TokenPromise class, it doesn't MUTATE anything
    // hence compiler will shift line 54 OUT of the while loop, leading to infinite loop
    // to fix this issue, ensure that isValid is std::atomic. Hence, NO ORDERING of any READ/WRITES of isValid can be done 
    // code will perform as written in Program Order
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