#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <optional>
#include <map>
#include <io.h>
#include <fcntl.h>

// PotionRecipe 클래스: 재료 목록을 vector<string>으로 변경
class PotionRecipe {
public:
    std::wstring potionName;
    std::vector<std::wstring> ingredients; // 단일 재료에서 재료 '목록'으로 변경

    // 생성자: 재료 목록을 받아 초기화하도록 수정
    PotionRecipe(const std::wstring& name, const std::vector<std::wstring>& ingredients)
        : potionName(name), ingredients(ingredients) {
    }
};

// AlchemyWorkshop 클래스: 레시피 목록을 관리
class AlchemyWorkshop {
private:
    std::vector<PotionRecipe> recipes;

public:
    // addRecipe 메서드: 재료 목록(vector)을 매개변수로 받도록 수정
    void addRecipe(const std::wstring& name, const std::vector<std::wstring>& ingredients) {
        recipes.push_back(PotionRecipe(name, ingredients));
        std::wcout << L">> 새로운 레시피 '" << name << L"'이(가) 추가되었습니다." << std::endl;
    }

    // 모든 레시피 출력 메서드
    void displayAllRecipes() const {
        if (recipes.empty()) {
            std::wcout << L"아직 등록된 레시피가 없습니다." << std::endl;
            return;
        }

        std::wcout << L"\n--- [ 전체 레시피 목록 ] ---" << std::endl;
        for (size_t i = 0; i < recipes.size(); ++i) {
            std::wcout << L"- 물약 이름: " << recipes[i].potionName << std::endl;
            std::wcout << L"  > 필요 재료: ";

            // 재료 목록을 순회하며 출력
            for (size_t j = 0; j < recipes[i].ingredients.size(); ++j) {
                std::wcout << recipes[i].ingredients[j];
                // 마지막 재료가 아니면 쉼표로 구분
                if (j < recipes[i].ingredients.size() - 1) {
                    std::wcout << ", ";
                }
            }
            std::wcout << std::endl;
        }
        std::wcout << L"---------------------------\n";
    }

    // 물약 이름으로 레시피 검색 메서드
    std::optional<PotionRecipe> searchRecipeByName(const std::wstring& name) const{
        auto it = std::find_if(recipes.begin(), recipes.end(), [name](const PotionRecipe& p) {
            return p.potionName == name;
            });

        if (it != recipes.end()) {
            return *it;
        }
        
        return std::nullopt;
    }

    // 재료로 레시피 검색 메서드: 해당 재료가 포함된 모든 레시피 검색
    std::vector<PotionRecipe> searchRecipeByIngredient(const std::wstring& ingredient) const{
        std::vector<PotionRecipe> result;

        for (int i = 0; i < recipes.size(); i++)
        {
            auto it = std::find_if(recipes[i].ingredients.begin(), recipes[i].ingredients.end(),
                [ingredient](const std::wstring i) {
                return i == ingredient;
                });

            if (it != recipes[i].ingredients.end())
            {
                result.push_back(recipes[i]);
            }
        }

        return result;
    }
};

// StockManager 클래스: 레시피 재고 관리
class StockManager {
private:
    std::map<std::wstring, int> potionStock;
    const int MAX_STOCK = 3;
    const AlchemyWorkshop& workshop;

public:
    // 생성자. 레시피 목록에 접근
    StockManager(const AlchemyWorkshop& myWorkshop) : workshop(myWorkshop){

    }

    // 자동 초기 재고 지급 메서드
    void initializeStock(const std::wstring& potionName) {
        potionStock[potionName] = MAX_STOCK;
        std::wcout << potionName << L" 초기 재고 지급. 현재 재고: " << potionStock[potionName] << std::endl;
    }

    // 물약 찾아서 지급: 포션 이름으로 찾기
    bool dispensePotion(const std::wstring& potionName) {
        auto it = workshop.searchRecipeByName(potionName);
        
        if (it != std::nullopt) {
            PotionRecipe recipe = it.value();

            if (potionStock[recipe.potionName] == 0) {
                std::wcout << L"현재 " << recipe.potionName << L"의 재고가 없습니다." << std::endl;
            }

            else {
                --potionStock[recipe.potionName];
                std::wcout << recipe.potionName << L"를 모험가에게 1개 주었습니다. 남은 재고: " << potionStock[recipe.potionName] << std::endl;
            }

            return true;
        }

        else {
            std::wcout << L"해당하는 이름의 레시피가 존재하지 않습니다." << std::endl;
            return false;
        }
    }
    
    // 물약 찾아서 지급: 구성 재료로 찾기(오버로딩)
    bool dispensePotion(const std::vector<std::wstring>& ingredients) {
        std::vector<PotionRecipe> recipes = workshop.searchRecipeByIngredient(ingredients[0]);
        std::wcout << recipes.size() << std::endl;

        for (int i = 1; i < ingredients.size(); ++i) {
            if (recipes.empty()) {
                std::wcout << L"입력하신 재료들로 만들 수 있는 레시피는 존재하지 않습니다." << std::endl;
                return false;
            }

            else {
                for (int j = recipes.size() - 1; j >= 0; --j) {
                    if (std::find(recipes[j].ingredients.begin(), recipes[j].ingredients.end(), ingredients[i]) == recipes[j].ingredients.end()) {
                        recipes.erase(recipes.begin() + j);
                    }
                }
            }
        }

        if (recipes.empty()) {
            std::wcout << L"입력하신 재료들로 만들 수 있는 레시피는 존재하지 않습니다." << std::endl;
            return false;
        }

        else {
            for (PotionRecipe& r : recipes) {
                --potionStock[r.potionName];
                std::wcout << r.potionName << L"를 모험가에게 1개 주었습니다. 남은 재고: " << potionStock[r.potionName] << std::endl;
            }

            return true;
        }
    }

    // 공병 반환 메서드: 재고 1추가
    void returnPotion(const std::wstring& potionName) {
        if (potionStock[potionName] >= 3) {
            std::wcout << L"현재 " << potionName << L"의 재고가 최대치입니다." << std::endl;
        }
        
        else {
            ++potionStock[potionName];
            std::wcout << L"공병 반환 완료!" << potionName << L"의 현재 재고: " << potionStock[potionName] << std::endl;
        }
    }

    // 재고 getter
    int getStock(const std::wstring& potionName) const{
        auto it = potionStock.find(potionName);
        
        if (it != potionStock.end()) {
            return it->second;
        }
        
        return 0;
    }
};

int main() {
    (void)_setmode(_fileno(stdout), _O_U16TEXT);
    (void)_setmode(_fileno(stdin), _O_U16TEXT);
    AlchemyWorkshop myWorkshop;
    StockManager myStockManager(myWorkshop);

    while (true) {
        std::wcout << L"⚗️ 연금술 공방 관리 시스템" << std::endl;
        std::wcout << L"1. 레시피 추가" << std::endl;
        std::wcout << L"2. 모든 레시피 출력" << std::endl;
        std::wcout << L"3. 물약 이름으로 레시피 검색" << std::endl;
        std::wcout << L"4. 재료 이름으로 레시피 검색" << std::endl;
        std::wcout << L"5. 이름으로 물약 찾기 및 지급" << std::endl;
        std::wcout << L"6. 재료로 물약 찾기 및 지급" << std::endl;
        std::wcout << L"7. 공병 반환하기" << std::endl;
        std::wcout << L"0. 종료" << std::endl;
        std::wcout << L"선택: ";

        int choice;
        std::wcin >> choice;

        if (std::wcin.fail()) {
            std::wcout << L"잘못된 입력입니다. 숫자를 입력해주세요." << std::endl;
            std::wcin.clear();
            std::wcin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            std::wstring name;
            std::wcout << L"물약 이름: ";
            std::wcin.ignore(10000, '\n');
            std::getline(std::wcin, name);

            // 여러 재료를 입력받기 위한 로직
            std::vector<std::wstring> ingredients_input;
            std::wstring ingredient;
            std::wcout << L"필요한 재료들을 입력하세요. (입력 완료 시 '끝' 입력)" << std::endl;

            while (true) {
                std::wcout << L"재료 입력: ";
                std::getline(std::wcin, ingredient);

                // 사용자가 '끝'을 입력하면 재료 입력 종료
                if (ingredient == L"끝") {
                    break;
                }
                ingredients_input.push_back(ingredient);
            }

            // 입력받은 재료가 하나 이상 있을 때만 레시피 추가
            if (!ingredients_input.empty()) {
                myWorkshop.addRecipe(name, ingredients_input);

                // 추가된 레시피 초기 재고 지급
                myStockManager.initializeStock(name);
            }
            else {
                std::wcout << L">> 재료가 입력되지 않아 레시피 추가를 취소합니다." << std::endl;
            }

        }
        else if (choice == 2) {
            myWorkshop.displayAllRecipes();

        }
        else if (choice == 3) {
            std::wstring name;
            std::wcout << L"물약 이름: ";
            std::wcin.ignore(10000, '\n');
            std::getline(std::wcin, name);
            auto result = myWorkshop.searchRecipeByName(name);

            if (result == std::nullopt) {
                std::wcout << L"해당 물약을 찾을 수 없습니다." << std::endl;
            }
            else {
                PotionRecipe recipe = result.value();
                std::wcout << L"\n--- [검색 결과] ---" << std::endl;
                std::wcout << L"- 물약 이름: " << recipe.potionName << std::endl;
                std::wcout << L"  > 필요 재료: ";

                for (size_t i = 0; i < recipe.ingredients.size(); ++i) {
                    std::wcout << recipe.ingredients[i];
                    if (i < recipe.ingredients.size() - 1) {
                        std::wcout << ", ";
                    }
                }
                std::wcout << std::endl;
                std::wcout << L"---------------------------\n";
            }
        }
        else if (choice == 4) {
            std::wstring name;
            std::wcout << L"재료 이름: ";
            std::wcin.ignore(10000, '\n');
            std::getline(std::wcin, name);
            std::vector<PotionRecipe> result = myWorkshop.searchRecipeByIngredient(name);

            if (result.size() == 0) {
                std::wcout << L"해당 재료가 포함된 물약을 찾을 수 없습니다." << std::endl;
            }
            else {
                std::wcout << L"\n--- [검색 결과] ---" << std::endl;

                for (const PotionRecipe& r: result)
                {
                    std::wcout << L"- 물약 이름: " << r.potionName << std::endl;
                    std::wcout << L"  > 필요 재료: ";

                    for (size_t i = 0; i < r.ingredients.size(); ++i) {
                        std::wcout << r.ingredients[i];
                        if (i < r.ingredients.size() - 1) {
                            std::wcout << ", ";
                        }
                    }

                    std::wcout << std::endl;
                }

                std::wcout << L"---------------------------\n";
            }
        }
        else if (choice == 5) {
            std::wstring name;
            std::wcout << L"반환할 포션 이름: ";
            std::wcin.ignore(10000, '\n');
            std::getline(std::wcin, name);
            myStockManager.dispensePotion(name);
        }
        else if (choice == 6) {
            std::vector<std::wstring> ingredients_input;
            std::wstring ingredient;
            std::wcout << L"구성 재료들을 입력하세요. (입력 완료 시 '끝' 입력)" << std::endl;
            std::wcin.ignore(10000, '\n');

            while (true) {
                std::wcout << L"재료 입력: ";
                std::getline(std::wcin, ingredient);

                // 사용자가 '끝'을 입력하면 재료 입력 종료
                if (ingredient == L"끝") {
                    break;
                }
                ingredients_input.push_back(ingredient);
            }

            // 입력받은 재료가 하나 이상 있을 때만 검색
            if (!ingredients_input.empty()) {
                for (std::wstring& i : ingredients_input) {
                    std::wcout << i << std::endl;
                }
                myStockManager.dispensePotion(ingredients_input);
            }
            else {
                std::wcout << L">> 재료가 입력되지 않아 지급을 취소합니다." << std::endl;
            }
        }
        else if (choice == 7) {
            std::wstring name;
            std::wcout << L"반환할 포션 이름: ";
            std::wcin.ignore(10000, '\n');
            std::getline(std::wcin, name);
            myStockManager.returnPotion(name);
        }
        else if (choice == 0) {
            std::wcout << L"공방 문을 닫습니다..." << std::endl;
            break;

        }
        else {
            std::wcout << L"잘못된 선택입니다. 다시 시도하세요." << std::endl;
        }
    }

    return 0;
}