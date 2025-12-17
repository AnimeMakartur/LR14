// ---
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <time.h>
// Константи для обмежень
#define MAX_OPT_TEXT 128 // Максимальна довжина тексту варіанту відповіді
#define MAX_OPTIONS 4 // Максимальна кількість варіантів відповіді
#define MAX_QUESTIONS 20 // Максимальна кількість питань у базі
#define MAX_DIFFICULTY 3 // Максимальний рівень складності
#define Q_WIDTH 50   // Ширина для стовпця "Питання"
#define D_WIDTH 12   // Ширина для стовпця "Складність"
#define A_WIDTH 15   // Ширина для стовпців "Відповідь 1/2/3/4"
#define R_WIDTH 14    // Ширина для стовпця "Правильна"
#define W_STAT 10 // Ширина для стовпця "Статус"

// Макроси для перевірки коректності введених даних
#define isValidLevel(level) ((level) >= 1 && (level) <= 3)
#define isValidNumOptions(num) ((num) >= 2 && (num) <= MAX_OPTIONS)
#define isValidNumQuestions(num) ((num) >= 1 && (num) <= MAX_QUESTIONS)

typedef struct QuestionsForQuestiniory
{
	int num;
    char Question[MAX_OPT_TEXT];
	int difficulty;
	char *Answer[MAX_OPTIONS];
	int RightAnswer;
} QFQ;

void createQuestionnaire(QFQ* pQuestiniory, int numQuestions) {
	char answer[MAX_OPT_TEXT];
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
			gets_s(answer, MAX_OPT_TEXT);
			pQuestiniory[i].Answer[j] = _strdup(answer);
			rewind(stdin);
		}
		printf("Enter number of the right answer (1-4): ");
		scanf_s("%d", &pQuestiniory[i].RightAnswer);
		rewind(stdin);
	}
}

void generateQuiz(const QFQ* pQuestiniory, int numQuestions, int* quizArray, int quizLen) {
	int questionsPerDifficulty = quizLen / MAX_DIFFICULTY;
	int remaining = quizLen % MAX_DIFFICULTY;

	int requiredCounts[MAX_DIFFICULTY + 1] = { 0 };
	for (int d = 1; d <= MAX_DIFFICULTY; d++) {
		requiredCounts[d] = questionsPerDifficulty;
		if (remaining > 0) {
			requiredCounts[d]++;
			remaining--;
		}
	}

	int count = 0;
	int attempts = 0;
	const int MAX_ATTEMPTS = numQuestions * 20; // Збільшуємо ліміт для надійності
	int randIndex;
	int difficulty;
	int alreadySelected;
	while (count < quizLen && attempts < MAX_ATTEMPTS) {
		attempts++;

		randIndex = rand() % numQuestions;
		difficulty = pQuestiniory[randIndex].difficulty;

		// Перевірка на унікальність (щоб не додати одне питання двічі)
		alreadySelected = 0;
		for (int i = 0; i < count; i++) {
			if (quizArray[i] == randIndex) {
				alreadySelected = 1;
				break;
			}
		}
		if (alreadySelected) continue;

		// ЗАПОБІЖНИК:
		// Якщо ми зробили багато спроб (наприклад, більше половини ліміту),
		// ми дозволяємо брати питання БУДЬ-ЯКОЇ складності, аби заповнити тест.
		int forceMode = (attempts > MAX_ATTEMPTS / 2);

		if (forceMode || (difficulty >= 1 && difficulty <= MAX_DIFFICULTY && requiredCounts[difficulty] > 0)) {
			quizArray[count] = randIndex;
			if (!forceMode) {
				requiredCounts[difficulty]--;
			}
			count++;
			// Не скидаємо attempts повністю, щоб "forceMode" не вимкнувся завчасно,
			// але даємо шанс алгоритму дозаповнити масив.
		}
	}

	// Якщо навіть після цього тест не повний (наприклад, quizLen > numQuestions)
	if (count < quizLen) {
		printf("\n[Warning] Could not find enough unique questions. Quiz limited to %d items.\n", count);
	}
}

void quizQuestion(QFQ* pQuestiniory, int* quizArray, const int quizLen, int* answers) {
	printf("\n=== Quiz Time! ===\n");
	int i, j;
	int* pIdx= quizArray;
	QFQ* currentQuestion=pQuestiniory;
	for (i = 0; pIdx != quizArray+ quizLen; pIdx++, i++) {
		printf("\nQuestion %d: %s\n", i + 1, (currentQuestion + *pIdx)->Question);
		for (j = 0; j < 4; j++) {
			printf(" %d. %s\n", j + 1, (currentQuestion + *pIdx)->Answer[j]);
		}

		printf("Your Answer: ");
		// Прямий запис у масив за індексом набагато безпечніший за арифметику вказівників
		scanf_s("%d", &answers[i]);

		rewind(stdin); // Обов'язково чистимо буфер після кожного вводу
	}
}

void checkTheResulrts(const QFQ* pQuestiniory, int* quizArray, int* userAnswers, const int quizLen) {
	if (quizLen <= 0) return;
	const char* status;
	const char* userAnsText;
	const char* correctAnsText;
	int correctCount = 0;
	int qIdx;
	int userChoice;
	int correctChoice;
	printf("\n\n%*s === QUIZ RESULTS TABLE ===\n", 20, "");
	// Заголовок таблиці
	printf("| %-2s | %-*s | %-*s | %-*s | %-*s |\n",
		"N", Q_WIDTH, "Question", A_WIDTH, "Your Answer", A_WIDTH, "Correct", W_STAT, "Status");

	for (int i = 0; i < quizLen; i++) {
		qIdx = quizArray[i];
		userChoice = userAnswers[i];
		correctChoice = pQuestiniory[qIdx].RightAnswer;

		status = (userChoice == correctChoice) ? "CORRECT" : "WRONG";
		if (userChoice == correctChoice) correctCount++;

		// Отримуємо тексти відповідей (з перевіркою на випадок некоректного вводу)
		userAnsText = (userChoice >= 1 && userChoice <= 4) ? pQuestiniory[qIdx].Answer[userChoice - 1] : "Invalid";
		correctAnsText = pQuestiniory[qIdx].Answer[correctChoice - 1];

		// Вивід рядка таблиці
		printf("| %-2d | %-50.50s | %-15.15s | %-15.15s | %-10s |\n",
			i + 1,
			pQuestiniory[qIdx].Question,
			userAnsText,
			correctAnsText,
			status);
	}
	// Підсумок під таблицею
	double percentage = ((float)correctCount / quizLen) * 100;
	printf("\nSUMMARY:\n");
	printf("Total Questions: %d\n", quizLen);
	printf("Correct Answers: %d\n", correctCount);
	printf("Success Rate:    %.2lf%%\n", percentage);
}


void printQuestionnaire(const QFQ* pQuestiniory, int numQuestions) {
	if (numQuestions <= 0) {
		printf("Анкету не знайдено або вона порожня.\n");
		return;
	}

	// --- 1. Вивід заголовка таблиці ---

	// Формат для заголовка: 
	// | № | Питання | Скл. | Відп. 1 | ... | Прав. |

	printf("| %-2s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*s |\n",
		"N", Q_WIDTH, "Question", D_WIDTH, "Dificult",
		A_WIDTH, "Answ. 1", A_WIDTH, "Answ. 2",
		A_WIDTH, "Answ. 3", A_WIDTH, "Answ. 4",
		R_WIDTH, "Right Answer");

	// Лінія розділення

	// --- 2. Вивід даних ---

	for (int i = 0; i < numQuestions; i++) {
		// Вивід основної інформації в один рядок
		printf("| %-2d | %-*.*s | %-*d |",
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
	}
}


int main()
{
	srand(time(NULL));
	QFQ questiniory[MAX_QUESTIONS];
	QFQ* pQuestiniory = questiniory;
	
	int arrQuiz[MAX_QUESTIONS], *pArrQuiz;
	int arrAnswers[MAX_QUESTIONS], * pArrAnswers;
	int userAnswer[MAX_QUESTIONS];
	pArrQuiz = arrQuiz;
	pArrAnswers = arrAnswers;
	int numQuestions = 0;
	int numQuestiniowyQuestions = 0;
	printf("Questionnaire Creation:\n");
	printf("Input number of questions: ");
	scanf_s("%d", &numQuestions);
	if (!isValidNumQuestions(numQuestions)) {
		printf("Invalid number of questions. Must be between 1 and %d.\n", MAX_QUESTIONS);
		return 1;
	}
	rewind(stdin);
	createQuestionnaire(pQuestiniory, numQuestions);
	printQuestionnaire(pQuestiniory, numQuestions);
	printf("Input number of questions for questiniory: ");
	scanf_s("%d", &numQuestiniowyQuestions);
	rewind(stdin);
	if (!isValidNumQuestions(numQuestiniowyQuestions) || numQuestiniowyQuestions>numQuestions) {
		printf("Invalid number of questiniory questions. Must be between 1 and %d.\n", numQuestions);
		return 1;
	}
	system("cls");
	generateQuiz(pQuestiniory, numQuestions, pArrQuiz, numQuestiniowyQuestions);
	quizQuestion(pQuestiniory, pArrQuiz, numQuestiniowyQuestions, userAnswer);
	checkTheResulrts(pQuestiniory, pArrQuiz, userAnswer, numQuestiniowyQuestions);
	return 0;
}