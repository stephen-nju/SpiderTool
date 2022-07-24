#ifndef SRC_EXTRACTOR_H
#define SRC_EXTRACTOR_H
#include <memory>
#include <string>
namespace spider {
    // 视频类型
enum class ContentType { UGC = 1, PGC, PUGV, Live, Comic };
struct Result {
    ContentType type_;
    int id_;
    std::string title_;
    Result(ContenType type,int id, const std::string& title) : type_(type),id_(id), title_(title) {}
    virtual ~Result() = default;
};

class Extractor {
private:
    std::unique_ptr<Result> result_;

public:
    Extractor();
    ~Extractor();
    std::unique_ptr<Result> getResult();
};
}  // namespace spider
#endif