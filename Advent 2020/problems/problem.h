#pragma once

#include <iostream>

#include <windows.h>

#define PROBLEM_CLASS(day_id, problem_id) problem_ ##day_id ## _ ## problem_id

#define PROBLEM_CLASS_INSTANTIATE(day_id, problem_id) new PROBLEM_CLASS(day_id, problem_id)(problem_id)

#define PROBLEM_CLASSES_INSTANTIATE(day_id) \
problem* problem_1_p = PROBLEM_CLASS_INSTANTIATE(day_id, 1); \
problem* problem_2_p = PROBLEM_CLASS_INSTANTIATE(day_id, 2);

#define PROBLEM_CLASSES_INSTANTIATE_AND_SOLVE(day_id, input_file) \
PROBLEM_CLASSES_INSTANTIATE(day_id) \
problem_1_p->solve(std::string(input_file)); \
problem_2_p->solve(std::string(input_file)); \
delete problem_1_p; \
delete problem_2_p; 

#define PROBLEM_CUSTOM_CLASS_H(day_id, problem_id) \
class PROBLEM_CLASS(day_id, problem_id): public problem  \

#define PROBLEM_BASIC_CLASS_H(day_id, problem_id) \
class PROBLEM_CLASS(day_id, problem_id): public problem { \
public: \
	PROBLEM_CLASS(day_id, problem_id)(int problem_num) : problem(problem_num) {}; \
	void solve(const std::string & file_name) override; \
}; \

#define PROBLEM_BASIC_CLASSES_H(day_id) \
PROBLEM_BASIC_CLASS_H(day_id, 1) \
PROBLEM_BASIC_CLASS_H(day_id, 2) \

#define PROBLEM_CLASS_CPP(day_id) \
using problem_1 = PROBLEM_CLASS(day_id, 1); \
using problem_2 = PROBLEM_CLASS(day_id, 2); \

class problem {
public:
	problem(int problem_num) : m_problem_number(problem_num) {};
	virtual void solve(const std::string& file_name) = 0;

	// Output the answer and automatically put it in the clipboard
	void output_answer(const std::string& answer) {
		if (answer.empty()) {
			return;
		}

		OpenClipboard(0);
		EmptyClipboard();
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, answer.size() + 1);
		if (!hg) {
			CloseClipboard();
			return;
		}
		memcpy(GlobalLock(hg), answer.c_str(), answer.size() + 1);
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);

		std::cout << "Problem " << m_problem_number << ": " << answer << "\n";
	}

	int m_problem_number;
};