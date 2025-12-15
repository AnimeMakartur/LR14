#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <time.h>

#define MAX_Q_TEXT 256 // Максимальна довжина тексту питання
#define MAX_OPT_TEXT 128 // Максимальна довжина тексту варіанту відповіді
#define MAX_OPTIONS 4 // Максимальна кількість варіантів відповіді
#define MAX_QUESTIONS 20 // Максимальна кількість питань у базі

#define isValidLevel(level) ((level) >= 1 && (level) <= 3)
#define isValidNumOptions(num) ((num) >= 2 && (num) <= MAX_OPTIONS)

struct Task1
{
	int num;
    char Question[MAX_Q_TEXT];
	int HardLvl;
	char Answer[MAX_OPTIONS][MAX_OPT_TEXT];
	int RightAnswer;
};

void createQuestionnaire(Task1* pQuestiniory, int numQuestions) {
	char answer1[MAX_OPT_TEXT];
	for(int i = 0; i < numQuestions; i++) {
		pQuestiniory[i].num = i + 1;
		printf("Enter question: ");
		gets_s(pQuestiniory[i].Question, MAX_OPT_TEXT);
		printf("Enter difficulty level (1-3): ");
		scanf_s("%d", &pQuestiniory[i].HardLvl);
		isValidLevel(pQuestiniory[i].HardLvl);
		rewind(stdin);
		printf("Enter 4 possible answers:\n");
		for (int j = 0; j < 4; j++) {
			printf("Answer %d: ", j + 1);
			gets_s(answer1, MAX_OPT_TEXT);
			strcpy_s(pQuestiniory[i].Answer[j], MAX_OPT_TEXT, answer1);
			rewind(stdin);
		}
		printf("Enter number of the right answer (1-4): ");
		scanf_s("%d", &pQuestiniory[i].RightAnswer);
		rewind(stdin);
	}
}


void generateQuiz(const Task1* pQuestiniory, int numQuestions, int* quizArray, int quizLen) {
	int selectedIndices[MAX_QUESTIONS] = { 0 };
	int count = 0;
	int randIndex;
	int alreadySelected;
	while (count < quizLen) {
		randIndex = rand() % numQuestions;
		alreadySelected = 0;
		for (int i = 0; i < count; i++) {
			if (selectedIndices[i] == randIndex) {
				alreadySelected = 1;
				break;
			}
		}
		if (!alreadySelected) {
			selectedIndices[count] = randIndex;
			quizArray[count] = randIndex;
			count++;
		}
	}
}


void quizQuestion(const Task1* pQuestiniory, int* quizArray, const int quizLen, int* answers) {
	int score = 0;
	int *userAnswer = answers;
	printf("Quiz Time!\n");
	int* currentQuestion = quizArray;
	int i;
	for (i = 0; currentQuestion != quizArray + quizLen; i++, currentQuestion++) {
		printf("Question: %s\n", pQuestiniory[*currentQuestion].Question);
		printf("Answers:");
		if (!scanf_s("%d", userAnswer+i)) {
			printf("Invalid input. Please enter a number between 1 and 4.\n");
			rewind(stdin);
			i--;
			currentQuestion--;
			continue;
		}
	}
}

void computeResults(const Task1* pQuestiniory, const int* quizArray, const int* userAnswers) {
	
}

void printQuestionnaire(const Task1* pQuestiniory, int numQuestions) {
	for (int i = 0; i < numQuestions; i++) {
		printf("Question %d: %s\n", i + 1, pQuestiniory[i].Question);
		printf("Difficulty Level: %d\n", pQuestiniory[i].HardLvl);
		printf("Answers:\n");
		for (int j = 0; j < 4; j++) {
			printf("%d. %s\n", j + 1, pQuestiniory[i].Answer[j]);
		}
		printf("Right Answer: %d\n", pQuestiniory[i].RightAnswer);
	}
}


int main()
{
	srand(time(NULL));
	struct Task1 questiniory[MAX_QUESTIONS];
	struct Task1* pQuestiniory = questiniory;
	int numQuestions = 0;
	printf("Questionnaire Creation:\n");
	printf("Input number of questions: ");
	scanf_s("%d", &numQuestions);
	if (!isValidNumOptions(numQuestions)) {
		printf("Invalid number of questions. Must be between 1 and %d.\n", MAX_QUESTIONS);
		free(questiniory);
		return 1;
	}
	rewind(stdin);
	createQuestionnaire(pQuestiniory, numQuestions);


	int randomIndex = rand() % numQuestions;
	printQuestionnaire(pQuestiniory, numQuestions);
	free(questiniory);
	return 0;
}