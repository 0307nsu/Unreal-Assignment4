#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <optional>
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
    std::optional<PotionRecipe> searchRecipeByName(const std::wstring& name) {
        auto it = std::find_if(recipes.begin(), recipes.end(), [name](const PotionRecipe& p) {
            return p.potionName == name;
            });

        if (it != recipes.end()) {
            return *it;
        }
        
        return std::nullopt;
    }

    // 재료로 레시피 검색 메서드: 해당 재료가 포함된 모든 레시피 검색
    std::vector<PotionRecipe> searchRecipeByIngredient(const std::wstring& ingredient) {
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

int main() {
    (void)_setmode(_fileno(stdout), _O_U16TEXT);
    (void)_setmode(_fileno(stdin), _O_U16TEXT);
    AlchemyWorkshop myWorkshop;

    while (true) {
        std::wcout << L"⚗️ 연금술 공방 관리 시스템" << std::endl;
        std::wcout << L"1. 레시피 추가" << std::endl;
        std::wcout << L"2. 모든 레시피 출력" << std::endl;
        std::wcout << L"3. 물약 이름으로 레시피 검색" << std::endl;
        std::wcout << L"4. 재료 이름으로 레시피 검색" << std::endl;
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
            std::wcout << L"필요한 재료들을 입력하세요. (입력 완료 시 'end' 입력)" << std::endl;

            while (true) {
                std::wcout << L"재료 입력: ";
                std::getline(std::wcin, ingredient);

                // 사용자가 '끝'을 입력하면 재료 입력 종료
                if (ingredient == L"end") {
                    break;
                }
                ingredients_input.push_back(ingredient);
            }

            // 입력받은 재료가 하나 이상 있을 때만 레시피 추가
            if (!ingredients_input.empty()) {
                myWorkshop.addRecipe(name, ingredients_input);
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