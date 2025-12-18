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
#define isValidNumOptions(num) ((num) >= 1 && (num) <= MAX_OPTIONS)
#define isValidNumQuestions(num) ((num) >= 1 && (num) <= MAX_QUESTIONS)

typedef struct QuestionsForQuestinioryAndItVeryLongNameLol
{
    char Question[MAX_OPT_TEXT];
	int difficulty;
	char *Answer[MAX_OPTIONS];
	int RightAnswer;
} QFQ;

void createQuestionnaire(QFQ* pQuestiniory, int* numQuestions) {
	char answer[MAX_OPT_TEXT];// Тимчасовий буфер для введення відповіді
	QFQ* currentQuestion = pQuestiniory;// Вказівник на поточне питання
	while(currentQuestion!= pQuestiniory+ MAX_QUESTIONS) {
		printf("Enter question: ");// Введення тексту питання
		gets_s(currentQuestion->Question, MAX_OPT_TEXT);
		if (currentQuestion->Question[0] == '\0') break; // Завершення вводу при порожньому рядку
		printf("Enter difficulty level (1-3): ");
		scanf_s("%d", &currentQuestion->difficulty);
		if (!isValidLevel(currentQuestion->difficulty)) {
			printf("Invalid difficulty level. Please enter a value between 1 and 3.\n");
			printf("Try again.\n");
			rewind(stdin);
			currentQuestion== pQuestiniory ? currentQuestion= pQuestiniory : currentQuestion--;// Повернення до попереднього питання
			continue;
		}
		rewind(stdin);
		printf("Enter 4 possible answers:\n");
		for (int j = 0; j < 4; j++) {
			printf("Answer %d: ", j + 1);
			gets_s(answer, MAX_OPT_TEXT);
			currentQuestion->Answer[j] = _strdup(answer);// Динамічне виділення пам'яті для відповіді
			rewind(stdin);// Очищення буфера вводу
		}
		printf("Enter number of the right answer (1-4): ");
		scanf_s("%d", &currentQuestion->RightAnswer);
		if (!isValidNumOptions(currentQuestion->RightAnswer)) {
			printf("Invalid answer number. Please enter a value between 1 and 4.\n");
			printf("Try again.\n");
			rewind(stdin);
			currentQuestion== pQuestiniory ? currentQuestion= pQuestiniory : currentQuestion--;// Повернення до попереднього питання
			continue;
		}
		rewind(stdin);
		currentQuestion++;// Перехід до наступного питання
		(*numQuestions)++;
	}
}

void generateQuiz(const QFQ* pQuestiniory, int numQuestions, int* quizArray, int quizLen) {
	int questionsPerDifficulty = quizLen / MAX_DIFFICULTY;// Базова кількість питань на кожен рівень складності
	int remaining = quizLen % MAX_DIFFICULTY;// Залишок для розподілу
	// Масив для відстеження скільки питань кожної складності ще потрібно додати
	int requiredCounts[MAX_DIFFICULTY + 1] = { 0 };
	// Ініціалізація requiredCounts
	for (int d = 1; d <= MAX_DIFFICULTY; d++) {
		requiredCounts[d] = questionsPerDifficulty;
		if (remaining > 0) {
			requiredCounts[d]++;
			remaining--;
		}
	}
	int forceMode;
	int count = 0;
	int attempts = 0;
	const int MAX_ATTEMPTS = numQuestions * 20; // Збільшуємо ліміт для надійності
	int randIndex;
	int difficulty;
	int alreadySelected;
	while (count < quizLen && attempts < MAX_ATTEMPTS) {
		attempts++;

		randIndex = rand() % numQuestions;// Випадковий індекс питання
		difficulty = pQuestiniory[randIndex].difficulty;// Визначення складності питання

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
		forceMode = (attempts > MAX_ATTEMPTS / 2);

		if ((attempts > MAX_ATTEMPTS / 2) || (difficulty >= 1 && difficulty <= MAX_DIFFICULTY && requiredCounts[difficulty] > 0)) {
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
	int i, j;// Лічильники
	int* pIdx = quizArray;// Вказівник на поточний індекс питання в масиві відібраних питань
	QFQ* currentQuestion = pQuestiniory;// Початок анкету
	int* panswers = answers;// Вказівник на масив відповідей користувача	
	for (i = 0; pIdx != quizArray+ quizLen; pIdx++, i++) {
		printf("\nQuestion %d: %s\n", i + 1, (currentQuestion + *pIdx)->Question);
		// Вивід варіантів відповіді
		for (j = 0; j < 4; j++) {
			printf(" %d. %s\n", j + 1, (currentQuestion + *pIdx)->Answer[j]);
		}

		printf("Your Answer: ");
		// Прямий запис у масив за індексом набагато безпечніший за арифметику вказівників
		if (!scanf_s("%d", panswers) || !isValidNumOptions(*panswers)) {
			printf("Invalid input. Please enter a number between 1 and 4.\n");
			printf("Try again.\n");
			rewind(stdin);
			pIdx--;   
			i--;
			panswers == answers ? panswers = answers : panswers--;// Повернення до попереднього питання
			continue;
		}
		panswers++;// Перехід до наступної відповіді
		rewind(stdin); // Обов'язково чистимо буфер після кожного вводу
	}
}

void printTheResulrts(const QFQ* pQuestiniory, int* quizArray, int* userAnswers, const int quizLen) {
	const char* status;// Статус відповіді
	const char* line = "------------------------------------------------------------------------------------------------------------";// Лінія розділення
	const char* userAnsText;// Текст відповіді користувача
	const char* correctAnsText;// Текст відповіді користувача
	int correctCount = 0;// Лічильник правильних відповідей
	int qIdx;// Індекс питання в оригінальній анкеті
	int userChoice;// Індекс питання в оригінальній анкеті
	int correctChoice;// Заголовок таблиці результатів
	printf("\n\n%*s === QUIZ RESULTS TABLE ===\n", 20, "");
	printf("%s\n", line);
	// Заголовок таблиці
	printf("| %-2s | %-*s | %-*s | %-*s | %-*s |\n",
		"N", Q_WIDTH, "Question", A_WIDTH, "Your Answer", A_WIDTH, "Correct", W_STAT, "Status");
	printf("%s\n", line);
	//Вивід рядка 
	for (int i = 0; i < quizLen; i++) {
		qIdx = quizArray[i];
		userChoice = userAnswers[i];
		correctChoice = pQuestiniory[qIdx].RightAnswer;

		status = (userChoice == correctChoice) ? "CORRECT" : "WRONG";
		if (userChoice == correctChoice) correctCount++;

		// Отримуємо тексти відповідей (з перевіркою на випадок некоректного вводу)
		userAnsText =pQuestiniory[qIdx].Answer[userChoice - 1];
		correctAnsText = pQuestiniory[qIdx].Answer[correctChoice - 1];

		// Вивід рядка таблиці
		printf("| %-2d | %-50.50s | %-15.15s | %-15.15s | %-10s |\n",
			i + 1,
			pQuestiniory[qIdx].Question,
			userAnsText,
			correctAnsText,
			status);
		printf("%s\n", line);
	}
	// Підсумок під таблицею
	double percentage = ((double)correctCount / quizLen) * 100;
	printf("\nSUMMARY:\n");
	printf("Total Questions: %d\n", quizLen);
	printf("Correct Answers: %d\n", correctCount);
	printf("Success Rate:    %.2lf%%\n", percentage);
}

void printQuestionnaire(const QFQ* pQuestiniory, int numQuestions) {
	// Перевірка на порожню анкету
	const char* line = "+----+----------------------------------------------------+--------------+-----------------+-----------------+-----------------+-----------------+----------------+";
	// --- 1. Вивід заголовка таблиці ---
	// Формат для заголовка: 
	// | № | Питання | Скл. | Відп. 1 | ... | Прав. |
	printf("%s\n", line);
	printf("| %-2s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*s |\n",
		"N", Q_WIDTH, "Question", D_WIDTH, "Dificult",
		A_WIDTH, "Answ. 1", A_WIDTH, "Answ. 2",
		A_WIDTH, "Answ. 3", A_WIDTH, "Answ. 4",
		R_WIDTH, "Right Answer");

	// Лінія розділення
	printf("%s\n", line);
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
		printf("%s\n", line);
	}
}

int main()
{
	srand(time(NULL));
	QFQ questiniory[MAX_QUESTIONS];// Масив для зберігання питань
	int arrQuiz[MAX_QUESTIONS];// Масив для зберігання індексів відібраних питань для тесту
	int userAnswer[MAX_QUESTIONS];// Масив для зберігання відповідей користувача
	int numQuestions = 0;// Кількість питань у анкеті
	int numQuestiniowyQuestions = 0;// Кількість питань у тесті
	printf("Questionnaire Creation:\n");
	createQuestionnaire(questiniory, &numQuestions);// Створення анкети
	if (!isValidNumQuestions(numQuestions)) {
		printf("No questions were added to the questionnaire.\n");
		return 1;
	}
	printQuestionnaire(questiniory, numQuestions);// Вивід анкети
	start:
	printf("Input number of questions for questiniory: ");
	scanf_s("%d", &numQuestiniowyQuestions);
	rewind(stdin);
	// Перевірка коректності введеної кількості питань для тесту
	if (!isValidNumQuestions(numQuestiniowyQuestions) || numQuestiniowyQuestions>numQuestions) {
		printf("Invalid number of questiniory questions. Must be between 1 and %d.\n", numQuestions);
		goto start;
	}
	system("cls");// Очистка екрану перед тестом
	generateQuiz(questiniory, numQuestions, arrQuiz, numQuestiniowyQuestions);// Генерація тесту
	quizQuestion(questiniory, arrQuiz, numQuestiniowyQuestions, userAnswer);// Проведення тесту
	printTheResulrts(questiniory, arrQuiz, userAnswer, numQuestiniowyQuestions);// Перевірка результатів тесту
	return 0;
}


