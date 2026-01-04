#ifndef UI_TREE_HPP
#define UI_TREE_HPP

#include "layout.hpp"
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <algorithm>

namespace ui {

    class Tree : public Widget {
    public:
        struct Item {
            Item(std::string const& text, Item* parent = nullptr)
                : parent_(parent), text_(text)
            {
                model_.setAlignmentW(0);
            }

            Item* parent_ = nullptr;
            std::vector<Item> children_;

            bool selected_ = false;
            bool expanded_ = false;

            std::string text_;
            uf::TextModel model_;
        };

    private:
        std::optional<Item> root_;

        std::function<void(Item*)> itemAdded;
        std::function<void(Item*)> itemRemoved;
        std::function<void(Item*)> itemSelected;

        int rowHeight_ = 40;

    public:
        Tree(std::string const& rootText)
            : root_(rootText)
        {
            root_->expanded_ = true; // root always visible
        }

        // ------------------------------------------------------------
        // Item Management
        // ------------------------------------------------------------

        Item* addItem(std::string const& parentText, std::string const& text) {
            Item* parent = findItem(&root_.value(), parentText);
            if (!parent)
                return nullptr;

            parent->children_.emplace_back(text, parent);
            Item* added = &parent->children_.back();

            if (itemAdded)
                itemAdded(added);

            return added;
        }

        void removeItem(std::string const& text) {
            Item* item = findItem(&root_.value(), text);
            if (!item || !item->parent_)
                return;

            auto& siblings = item->parent_->children_;
            siblings.erase(
                std::remove_if(siblings.begin(), siblings.end(),
                    [&](Item const& c) { return c.text_ == text; }),
                siblings.end()
            );

            if (itemRemoved)
                itemRemoved(item);
        }

        void selectItem(std::string const& text) {
            Item* item = findItem(&root_.value(), text);
            if (!item)
                return;

            item->selected_ = true;
            if (itemSelected)
                itemSelected(item);
        }

        // ------------------------------------------------------------
        // Rendering
        // ------------------------------------------------------------

        void onPaint(Canvas* c) override {
            c->solid(col.darkGray);
            c->solid(col.black, 4);
            c->rect(x(), y(), w(), h());

            int row = 0;
            for (Item* item : visibleItems()) {
                int iy = y() + row * rowHeight_;

                if (item->selected_) {
                    c->solid(col.flutterBlue);
                    c->rect(x(), iy, w(), rowHeight_);
                }

                c->solid(col.white);
                int indent = depth(item) * 20;
                c->text(item->model_, item->text_, 48,
                    x() + indent, iy, w() - indent, rowHeight_);

                row++;
            }
        }

        // ------------------------------------------------------------
        // Mouse Interaction
        // ------------------------------------------------------------

        void onMousePress(MouseEvent* me) override {
            if (!me->left())
                return;

            if (Item* item = itemAt(me->x(), me->y())) {
                item->selected_ = !item->selected_;
                if (itemSelected)
                    itemSelected(item);
            }
        }

        void onMouseDoublePress(MouseEvent* me) override {
            if (!me->left())
                return;

            if (Item* item = itemAt(me->x(), me->y())) {
                item->expanded_ = !item->expanded_;
            }
        }

    private:
        // ------------------------------------------------------------
        // Hit Testing
        // ------------------------------------------------------------

        Item* itemAt(int px, int py) {
            int row = 0;
            for (Item* item : visibleItems()) {
                int iy = y() + row * rowHeight_;
                if (py >= iy && py < iy + rowHeight_)
                    return item;
                row++;
            }
            return nullptr;
        }

        // ------------------------------------------------------------
        // Item Lookup
        // ------------------------------------------------------------

        Item* findItem(Item* item, std::string const& text) {
            if (item->text_ == text)
                return item;

            for (auto& child : item->children_) {
                if (Item* found = findItem(&child, text))
                    return found;
            }
            return nullptr;
        }

        // ------------------------------------------------------------
        // Visible Item List (respects expanded_)
        // ------------------------------------------------------------

        std::vector<Item*> visibleItems() {
            std::vector<Item*> out;
            collectVisible(&root_.value(), out);
            return out;
        }

        void collectVisible(Item* item, std::vector<Item*>& out) {
            out.push_back(item);

            if (!item->expanded_)
                return;

            for (auto& child : item->children_)
                collectVisible(&child, out);
        }

        // ------------------------------------------------------------
        // Utility
        // ------------------------------------------------------------

        int depth(Item* item) {
            int d = 0;
            while (item->parent_) {
                d++;
                item = item->parent_;
            }
            return d;
        }
    };

} // namespace ui

#endif // UI_TREE_HPP
