// ---
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <time.h>
// Константи для обмежень
#define MAX_Q_TEXT 50 // Максимальна довжина тексту питання
#define MAX_OPT_TEXT 128 // Максимальна довжина тексту варіанту відповіді
#define MAX_OPTIONS 4 // Максимальна кількість варіантів відповіді
#define MAX_QUESTIONS 20 // Максимальна кількість питань у базі
#define MAX_DIFFICULTY 3 // Максимальний рівень складності
#define Q_WIDTH 50   // Ширина для стовпця "Питання"
#define D_WIDTH 12   // Ширина для стовпця "Складність"
#define A_WIDTH 50   // Ширина для стовпців "Відповідь 1/2/3/4"
#define R_WIDTH 14    // Ширина для стовпця "Правильна"

// Макроси для перевірки коректності введених даних
#define isValidLevel(level) ((level) >= 1 && (level) <= 3)
#define isValidNumOptions(num) ((num) >= 2 && (num) <= MAX_OPTIONS)
#define isValidNumQuestions(num) ((num) >= 1 && (num) <= MAX_QUESTIONS)

struct Task1
{
	int num;
    char Question[MAX_Q_TEXT];
	int difficulty;
	char *Answer[MAX_OPTIONS];
	int RightAnswer;
};

void createQuestionnaire(Task1* pQuestiniory, int numQuestions) {
	char answer1[MAX_OPT_TEXT];
	for(int i = 0; i < numQuestions; i++) {
		pQuestiniory[i].num = i + 1;
		printf("Enter question: ");
		gets_s(pQuestiniory[i].Question, MAX_OPT_TEXT);
		printf("Enter difficulty level (1-3): ");
		scanf_s("%d", &pQuestiniory[i].difficulty);
		isValidLevel(pQuestiniory[i].difficulty);
		rewind(stdin);
		printf("Enter 4 possible answers:\n");
		for (int j = 0; j < 4; j++) {
			printf("Answer %d: ", j + 1);
			gets_s(answer1, MAX_OPT_TEXT);
			pQuestiniory[i].Answer[j]= _strdup(answer1);
			rewind(stdin);
		}
		printf("Enter number of the right answer (1-4): ");
		scanf_s("%d", &pQuestiniory[i].RightAnswer);
		rewind(stdin);
	}
}

void generateQuiz(const Task1* pQuestiniory, int numQuestions, int* quizArray, int quizLen) {
	int questionsPerDifficulty = quizLen / MAX_DIFFICULTY;
	int remaining = quizLen % MAX_DIFFICULTY;

	int requiredCounts[MAX_DIFFICULTY + 1] = { 0 };
	for (int d = 1; d <= MAX_DIFFICULTY; d++) {
		requiredCounts[d] = questionsPerDifficulty;
		if (remaining > 0) {
			requiredCounts[d]++; // Розподіляємо залишок
			remaining--;
		}
	}
	// 2. Випадковий вибір з перевіркою квоти та унікальності
	int selectedIndices[MAX_QUESTIONS] = { 0 }; // Для відстеження унікальності
	int count = 0; // Кількість обраних питань
	int attempts = 0; // Для запобігання нескінченному циклу, якщо неможливо знайти питання

	while (count < quizLen && attempts < numQuestions * 5) { // Обмеження спроб
		attempts++;

		// Вибираємо випадковий індекс
		int randIndex = rand() % numQuestions;
		int difficulty = pQuestiniory[randIndex].difficulty;

		// A. Перевірка, чи не перевищує складність квоту
		if (difficulty >= 1 && difficulty <= MAX_DIFFICULTY) {
			if (requiredCounts[difficulty] <= 0) {
				continue;
			}
		}
		else {
			continue;
		}
		// B. Перевірка, чи це питання вже обрано (логіка з вашого оригінального коду)
		int alreadySelected = 0;
		for (int i = 0; i < count; i++) {
			if (selectedIndices[i] == randIndex) {
				alreadySelected = 1;
				break;
			}
		}

		// C. Якщо питання унікальне та в межах квоти, додаємо його
		if (!alreadySelected) {
			selectedIndices[count] = randIndex;
			quizArray[count] = randIndex;
			// Зменшуємо необхідну кількість для цього рівня складності
			requiredCounts[difficulty]--;
			count++;
			attempts = 0; // Скидаємо лічильник спроб після успішного вибору
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
		if (scanf_s("%d", userAnswer+i)==0) {
			printf("Invalid input. Please enter a number between 1 and 4.\n");
			rewind(stdin);
			i--;
			currentQuestion--;
			continue;
		}
	}
}

void checkTheResulrts(const Task1* pQuestiniory, int* quizArray, int* userAnswers, const int quizLen) {
	
}

int computeResults(int* quizArray, int* userAnswers, const int quizLen) {
	int score = 0;
	int* currentQuestion = quizArray;
	int* userAnswer = userAnswers;
	for(; currentQuestion != quizArray + quizLen; currentQuestion++, userAnswer++) {
		if (*userAnswers== *quizArray) {
			score++;
		}
	}
	return score;
}

void printQuestionnaire(const Task1* pQuestiniory, int numQuestions) {
	if (numQuestions <= 0) {
		printf("Анкету не знайдено або вона порожня.\n");
		return;
	}

	// --- 1. Вивід заголовка таблиці ---

	// Формат для заголовка: 
	// | № | Питання | Скл. | Відп. 1 | ... | Прав. |
	printf("+-----+");
	for (int i = 0; i < Q_WIDTH + D_WIDTH + 4 * A_WIDTH + R_WIDTH + 20; i++) printf("-");
	printf("+\n");

	printf("| %-3s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*s |\n",
		"N", Q_WIDTH, "Question", D_WIDTH, "Dificult",
		A_WIDTH, "Answ. 1", A_WIDTH, "Answ. 2",
		A_WIDTH, "Answ. 3", A_WIDTH, "Answ. 4",
		R_WIDTH, "Right Answer");

	// Лінія розділення
	printf("+-----+");
	for (int i = 0; i < Q_WIDTH + D_WIDTH + 4 * A_WIDTH + R_WIDTH + 20; i++) printf("-");
	printf("+\n");

	// --- 2. Вивід даних ---

	for (int i = 0; i < numQuestions; i++) {
		// Вивід основної інформації в один рядок
		printf("| %-3d | %-*.*s | %-*d |",
			i + 1,
			Q_WIDTH, Q_WIDTH, pQuestiniory[i].Question, // Обрізання тексту питання
			D_WIDTH, pQuestiniory[i].difficulty);

		// Вивід відповідей
		for (int j = 0; j < 4; j++) {
			// Обрізання тексту відповіді
			printf(" %-*.*s |", A_WIDTH, A_WIDTH, pQuestiniory[i].Answer[j]);
		}

		// Вивід правильної відповіді
		printf(" %-*d |\n", R_WIDTH, pQuestiniory[i].RightAnswer);

		// Додаткова лінія розділення між записами
		printf("+-----+");
		for (int k = 0; k < Q_WIDTH + D_WIDTH + 4 * A_WIDTH + R_WIDTH + 20; k++) printf("-");
		printf("+\n");
	}
}


int main()
{
	srand(time(NULL));
	struct Task1 questiniory[MAX_QUESTIONS];
	struct Task1* pQuestiniory = questiniory;
	
	int arrQuiz[MAX_QUESTIONS], *pArrQuiz;
	int arrAnswers[MAX_QUESTIONS], * pArrAnswers;
	pArrQuiz = arrQuiz;
	pArrAnswers = arrAnswers;
	int numQuestions = 0;
	int numQuestiniowyQuestions = 0;
	printf("Questionnaire Creation:\n");
	printf("Input number of questions: ");
	scanf_s("%d", &numQuestions);
	if (!isValidNumOptions(numQuestions)) {
		printf("Invalid number of questions. Must be between 1 and %d.\n", MAX_QUESTIONS);
		return 1;
	}
	rewind(stdin);
	createQuestionnaire(pQuestiniory, numQuestions);
	printf("Input number of questions for questiniory: ");
	scanf_s("%d", &numQuestiniowyQuestions);
	rewind(stdin);
	if (!isValidNumQuestions(numQuestiniowyQuestions) || numQuestiniowyQuestions>numQuestions) {
		printf("Invalid number of questiniory questions. Must be between 1 and %d.\n", numQuestions);
		return 1;
	}
	generateQuiz(pQuestiniory, numQuestions, pArrQuiz, numQuestiniowyQuestions);
	/*quizQuestion(pQuestiniory, pArrQuiz, numQuestiniowyQuestions, pArrAnswers);
	int result = computeResults(pArrQuiz, pArrAnswers, numQuestiniowyQuestions);
	printf("Your score: %d out of %d\n", result, numQuestiniowyQuestions);*/
	printQuestionnaire(pQuestiniory, numQuestions);
	return 0;
}