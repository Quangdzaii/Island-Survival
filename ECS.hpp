#pragma once
#include <iostream>
#include <vector>
#include <memory> // Hỗ trợ con trỏ thông minh
#include <algorithm>
#include <bitset> // Biểu diễn và thao tác trên dãy bit cố định
#include <array>

using namespace std;

// Khai báo tạm vị chưa có class đầy đủ của chúng
class Component;
class Entity;
class Manager;

using ComponentID = size_t;
using Group = size_t;

// Hàm inline cho thấy nó là hàm nhỏ và sẽ được gọi nhiều lần, thường dùng trong header
inline ComponentID getNewComponentTypeID() {
    static ComponentID lastID = 0u;
    return lastID++;
}

// Gán id cho mỗi kiểu dữ liệu cụ thể, template viết một hàm hoặc lớp chung cho nhiều kiểu dữ liệu khác nhau
template <typename T> inline ComponentID getComponentTypeID() noexcept {
    static_assert(is_base_of<Component, T>::value, ""); // Lệnh kiểm tra tại thời điểm biên dịch
    static ComponentID typeID = getNewComponentTypeID();
    return typeID;
}

constexpr size_t maxComponents = 32; // constexpr để khai báo hằng số tại thời điểm biên dịch
constexpr size_t maxGroups = 32;

using ComponentBitset = bitset<maxComponents>; // Một dãy bit để kiểm tra xem entity nào có những component nào
using GroupBitset = bitset<maxGroups>;

using ComponentArray = array<Component*, maxComponents>; // mảng các con trỏ tới component, truy cập trực tiếp component theo id

class Component{
public:
    Entity* entity; // Để biết component thuộc entity nào

    virtual void init() {}
    virtual void update() {}
    virtual void draw() {}

    virtual ~Component() {}
};

class Entity {
public:
    Entity(Manager& mManager) : manager(mManager) {}

    void update() {
        for(auto& c : components) c -> update();
    }

    void draw() {
        for(auto& c : components) c -> draw();
    }
    bool isActive() const {return active;}
    void destroy() {active = false;}

    bool hasGroup(Group mGroup) {
        return groupBitset[mGroup];
    }

    void addGroup(Group mGroup);
    void delGroup(Group mGroup) {
        groupBitset[mGroup] = false;
    }

    template <typename T> bool hasComponent() const {
        return componentBitset[getComponentTypeID<T>()];
    }

    template <typename T, typename... TArgs>
    T& addComponent(TArgs&&... mArgs) { // // TArgs&&...: nhận nhiều tham số khởi tạo (hàm dựng)
        T* c(new T(forward<TArgs>(mArgs)...));
        c -> entity = this;
        unique_ptr<Component> uPtr{c}; // unique_ptr để quản lý tự động bộ nhớ
        components.emplace_back(move(uPtr));

        componentArray[getComponentTypeID<T>()] = c;
        componentBitset[getComponentTypeID<T>()] = true;

        c -> init();
        return *c;
    }

    template<typename T> T& getComponent() const {
        auto ptr(componentArray[getComponentTypeID<T>()]);
        return *static_cast<T*>(ptr);
    }

private:
    Manager& manager;
    bool active = true;
    vector<unique_ptr<Component>> components;

    // dùng để truy cập nhanh component cụ thể
    ComponentArray componentArray;
    ComponentBitset componentBitset;
    GroupBitset groupBitset;
};

class Manager {
public:
    void update() {
        for(auto& e : entities) e -> update();
    }

    void draw() {
        for(auto& e : entities) e -> draw();
    }

    void refresh() {
        for(auto i(0u); i < maxGroups; i++) { // 0u là unsigned int 0, để tránh warning khi so sánh với size_t
            auto& v(groupedEntities[i]); // Truy xuất tham chiếu đến vector của nhóm thứ i
            v.erase(
                    remove_if(begin(v), end(v), // remove_if(...): đưa các phần tử cần xóa về cuối vector
                              [i](Entity* mEntity) {
                                  return !mEntity -> isActive() || !mEntity -> hasGroup(i);
                              }),
                                  end(v));
        }

        entities.erase(remove_if(begin(entities), end(entities),
            [](const unique_ptr<Entity> &mEntity) {
                    return !mEntity -> isActive();
                }),
                end(entities));
    }

    void AddToGroup(Entity* mEntity, Group mGroup) {
        groupedEntities[mGroup].emplace_back(mEntity); // emplace_back được sử dụng để thêm một đối tượng vào cuối vector
    // emplace_back thường được sử dụng thay vì push_back vì nó có thể tạo đối tượng trực tiếp tại chỗ mà không cần phải tạo bản sao
    }

    vector<Entity*>& getGroup(Group mGroup) {
       return groupedEntities[mGroup];
    }

    Entity& addEntity() {
       Entity* e = new Entity(*this);
       unique_ptr<Entity> uPtr{e};
       entities.emplace_back(move(uPtr));
       return *e;
    }

private:
    vector<unique_ptr<Entity>> entities;
    array<vector<Entity*>, maxGroups> groupedEntities;
};
