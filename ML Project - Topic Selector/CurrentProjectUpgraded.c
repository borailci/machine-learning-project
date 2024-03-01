#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#define ITER 1000
#define ITER2 10000
#define ITER3 500
#define MAX_WORD_SIZE 250

typedef struct // Dictionary struct variable.
{
    char **dictionary;
    int wordCount;
    int size;

} wordList;

typedef struct // hotVectors struct variable.
{
    double **vectorList;
    int vectorCount;

} hotVectors;

bool isWordInList(wordList *wordList, char *word);
void fillWordList(wordList *wordList, char *filename);
void trimPunctuation(char *word);
void createVector(wordList *wordList, char *sentence, hotVectors *vectors, int sentenceIndex);
void findSentences(wordList *wordList, hotVectors *vectors, char *filename);
bool isWordInList(wordList *wordList, char *word);
void fillWordList(wordList *wordList, char *filename);
void allocateList(wordList *list);
void allocateVectors(hotVectors *vectors, int wordCount, int vectorCount);
void printDictionary(wordList *list);
void createRealData(double *realY);
void createTestRealData(double *testData);
void gradientDescent(wordList *list, double *realData, hotVectors *vector, double *w);
void ADAM(wordList *list, double *realData, hotVectors *vector, double *w);
void stochasticGradientDescent(wordList *list, double *realData, hotVectors *vector, double *w);
void calculateResultFromMethods(double *w, hotVectors *testData, double *realData, int wordCount);
void assignParameters(double *parameters, double assignValue, int len);
void trainMethods(wordList *dictionary, double *realData, hotVectors *vectors, double ***initialParameters, double *initialValue);
void writeParameters(char *filename, double ***initialParameters, int wordCount);
void writeErrors(double errors[], int iter);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    //  DECLARATIONS & ALLOCATIONS //
    srand(time(NULL));
    int i, j;
    char *classA = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
    char *classB = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
    char *test = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
    char *trainingData = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
    char *excelfile = "wparametersfinal.csv";
    wordList *dictionary = (wordList *)malloc(sizeof(wordList));
    hotVectors *vectors = (hotVectors *)malloc(sizeof(hotVectors));
    hotVectors *vectorsTest = (hotVectors *)malloc(sizeof(hotVectors));
    allocateList(dictionary);
    //  DECLARATIONS & ALLOCATIONS //

    // GETTING THE FILE NAMES //
    printf("\n# Give the dictionary data for class A : ");
    scanf("%s", classA);
    printf("\nEntered file name : %s", classA);
    printf("\n# Give the dictionary data for class B : ");
    scanf("%s", classB);
    printf("\nEntered file name : %s", classB);
    printf("\n# Give the training data we will create hot vector : ");
    scanf("%s", trainingData);
    printf("\nEntered file name : %s", trainingData);
    printf("\n# Give the test data : ");
    scanf("%s", test);
    printf("\nEntered file name : %s", test);
    // system("cls");
    //  GETTING THE FILE NAMES //

    // CREATING THE DICTIONARY FROM CLASS A AND CLASS B //
    fillWordList(dictionary, classA);
    // system("cls");
    fillWordList(dictionary, classB);
    // system("cls");
    //  CREATING THE DICTIONARY FROM CLASS A AND CLASS B //

    // ALLOCATING VECTORS
    allocateVectors(vectors, dictionary->wordCount, MAX_WORD_SIZE);
    allocateVectors(vectorsTest, dictionary->wordCount, MAX_WORD_SIZE);
    // ALLOCATING VECTORS

    // CREATING HOT VECTORS FROM TRAINING DATA
    findSentences(dictionary, vectors, trainingData);
    findSentences(dictionary, vectorsTest, test);
    // CREATING HOT VECTORS FROM TRAINING DATA

    // ASSIGNING INITIAL PARAMETERS
    double ***initialParameters = (double ***)calloc(3, sizeof(double **));
    double *initialValue = (double *)calloc(5, sizeof(double));

    initialValue[0] = 0.0;
    initialValue[1] = 0.02;
    initialValue[2] = -0.02;
    initialValue[3] = 0.1;
    initialValue[4] = -0.3;

    for (j = 0; j < 3; j++)
    {
        initialParameters[j] = (double **)calloc(5, sizeof(double **));
        for (i = 0; i < 5; i++)
        {
            initialParameters[j][i] = (double *)calloc(dictionary->wordCount, sizeof(double));
            assignParameters(initialParameters[j][i], initialValue[i], dictionary->wordCount);
        }
    }
    // ASSIGNING INITIAL PARAMETERS

    // ASSIGNING REAL AND TEST DATA FOR DEVELOPMENT
    double *realData = (double *)calloc(160, sizeof(double));
    double *testData = (double *)calloc(40, sizeof(double));
    createRealData(realData);
    createTestRealData(testData);
    // ASSIGNING REAL AND TEST DATA FOR DEVELOPMENT

    // TRAINING ALL METHODS FOR DIFFERENT INITIAL VALUES
    trainMethods(dictionary, realData, vectors, initialParameters, initialValue);
    // TRAINING ALL METHODS FOR DIFFERENT INITIAL VALUES
    // Sleep(1000);

    // TESTING ALL METHODS WITH TEST DATA
    for (i = 0; i < 5; i++)
    {
        calculateResultFromMethods(initialParameters[0][i], vectorsTest, testData, dictionary->wordCount);
        printf("\nGRADIENT DESCENT -> Given Initial Value : %lf", initialValue[i]);
        Sleep(5000);
        calculateResultFromMethods(initialParameters[1][i], vectorsTest, testData, dictionary->wordCount);
        printf("\nSTOHASTIC GRADIENT DESCENT -> Given Initial Value : %lf", initialValue[i]);
        Sleep(5000);
        calculateResultFromMethods(initialParameters[2][i], vectorsTest, testData, dictionary->wordCount);
        printf("\nADAM -> Given Initial Value : %lf", initialValue[i]);
        Sleep(5000);
    }
    // TESTING ALL METHODS WITH TEST DATA

    // WRITES FINAL PARAMETERS INTO A FILE
    writeParameters(excelfile, initialParameters, dictionary->wordCount);
    // WRITES FINAL PARAMETERS INTO A FILE

    return 0;
    //  END OF THE CODE
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double dtanh(double x)
{
    double coshx = cosh(x);
    return 1.0 / (coshx * coshx);
}
void trimPunctuation(char *word)
{
    int i = 0;
    while (i < strlen(word))
    {
        while (word[i] != '\0' && !ispunct(word[i]) && word[i] != '\n')
        {
            i++;
        }
        int j = i;
        // Trim trailing punctuation
        while (j < strlen(word))
        {
            word[j] = word[j + 1];
            j++;
        }
        word[j + 1] = '\0';
    }
    word[i + 1] = '\0';
}

void createVector(wordList *wordList, char *sentence, hotVectors *vectors, int sentenceIndex)
{
    int head, tail, j, i;
    head = 0;
    tail = 0;
    char buffer[500];
    while (head < strlen(sentence))
    {
        j = 0;
        while (isalpha(sentence[tail]))
        {
            buffer[j] = sentence[tail];
            tail++;
            j++;
        }
        buffer[j] = '\0';
        for (i = 0; i < wordList->wordCount; i++)
        {
            if (!strcmp(buffer, wordList->dictionary[i]))
            {
                vectors->vectorList[sentenceIndex][i] = 1;
                break;
            }
        }
        tail++;
        head = tail;
    }
}

void findSentences(wordList *wordList, hotVectors *vectors, char *filename)
{
    int sentenceCount = 0;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }
    char sentence[500];
    int c;
    int sentenceIndex = 0;
    while ((c = fgetc(file)) != EOF)
    {
        sentence[sentenceIndex++] = c;
        // Checking if we've reached the end of a sentence
        if (c == '.' || c == '?' || c == '!')
        {
            c = fgetc(file);
            if (!isdigit(c))
            {
                sentence[sentenceIndex++] = '\0';
                trimPunctuation(sentence);
                strlwr(sentence);
                createVector(wordList, sentence, vectors, sentenceCount); // Creating vector if a sentence is found
                sentenceCount = sentenceCount + 1;
                sentenceIndex = 0;
                memset(sentence, 0, sizeof(sentence));
            }
            sentence[sentenceIndex++] = c;
        }
    }

    fclose(file);
    vectors->vectorCount = sentenceCount;
    printf("\n%d vectors are created from file %s. Process completed.", vectors->vectorCount, filename);
}

bool isWordInList(wordList *wordList, char *word) // Checks if the word is in list.
{
    int i = 0;
    while (i < wordList->wordCount && strcmp(word, wordList->dictionary[i]))
    {
        i++;
    }
    if (i < wordList->wordCount)
    {
        return true;
    }
    else
        return false;
}

void fillWordList(wordList *wordList, char *filename) // Reading a file and putting words to dictionary
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("\n!ERROR! File cannot read!");
        return;
    }
    printf("\n%s is reading, please wait...", filename);
    char *currentWord = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
    while (fscanf(file, "%s", currentWord) != EOF)
    {
        trimPunctuation(currentWord);
        currentWord = strlwr(currentWord);
        if (!isdigit(currentWord[0]) && !isWordInList(wordList, currentWord))
        {
            if (wordList->wordCount + 1 > wordList->size)
            {
                wordList->size += 100;
                wordList->dictionary = realloc(wordList->dictionary, (wordList->size) * sizeof(char *));
                int i;
                for (i = wordList->wordCount; i < wordList->wordCount + 100; i++)
                    wordList->dictionary[i] = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
            }
            strcpy(wordList->dictionary[wordList->wordCount], currentWord);
            wordList->wordCount++;
        }
    }
    fclose(file);
    printf("\nWords added from %s. File closed successfully.", filename);
}

void allocateList(wordList *list) // Allocating memory for dictionary
{
    int i, j;
    list->wordCount = 0;
    list->size = MAX_WORD_SIZE;
    list->dictionary = (char **)malloc(sizeof(char *) * MAX_WORD_SIZE);
    for (i = 0; i < MAX_WORD_SIZE; i++)
    {
        list->dictionary[i] = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
    }
}
void allocateVectors(hotVectors *vectors, int wordCount, int vectorCount) // Allocating memory for hot vectorss
{
    int i, j;
    vectors->vectorList = (double **)malloc(sizeof(double *) * vectorCount);
    for (i = 0; i < vectorCount; i++)
    {
        vectors->vectorList[i] = (double *)malloc(sizeof(double) * wordCount);
    }
}
void printDictionary(wordList *list)
{
    system("cls");
    printf("\n$$$ DICTIONARY $$$\n");
    int i;
    for (i = 0; i < list->wordCount; i++)
    {
        printf("\n-> Word %d : %s", i + 1, list->dictionary[i]);
    }
}

void createHotVector(wordList *list, double *vector)
{
    char *currentText = (char *)calloc(10000, sizeof(char));

    int i;
    for (i = 0; i < list->wordCount; i++)
    {
    }
}

void createRealData(double *realY)
{
    int i;
    for (i = 0; i < 160; i++)
    {
        if (i < 80)
            realY[i] = 1;
        else
            realY[i] = -1;
    }
    printf("\nReal data has been created.");
}
void createTestRealData(double *testData)
{
    int i;
    for (i = 0; i < 40; i++)
    {
        if (i < 20)
            testData[i] = 1.0;
        else
            testData[i] = -1.0;
    }
    printf("\nTest data has been created.");
}

void gradientDescent(wordList *list, double *realData, hotVectors *vector, double *w) // Gradient Descent Algorithm
{
    int i, j, k;
    double LR = 0.03;
    double grad, wx = 0;
    double errors[ITER];
    double error = 0;
    clock_t start = clock();
    for (i = 0; i < ITER; i++)
    {
        for (j = 0; j < vector->vectorCount; j++) // Sentence count
        {
            wx = 0;
            for (k = 0; k < list->wordCount; k++) // Word count
            {
                wx += w[k] * vector->vectorList[j][k];
            }

            grad = ((realData[j] - tanh(wx)));
            for (k = 0; k < list->wordCount; k++)
            {
                w[k] = w[k] - ((LR * grad * dtanh(wx) * vector->vectorList[j][k]) * (-2.0 / list->wordCount)); // Gradient Descent implementation
            }
            error += grad * grad;
        }
        errors[i] = error / vector->vectorCount; // Calculating errors
        error = 0;
    }
    clock_t end = clock();
    double execTime = (double)(end - start) / CLOCKS_PER_SEC; // Calculating time
    printf("\nExecution time GD: %lf", execTime);
}

void stochasticGradientDescent(wordList *list, double *realData, hotVectors *vector, double *w)
{
    int i, j, k, l;
    double LR = 0.3;
    double grad, wx = 0;
    double errors[ITER2];
    double error = 0;
    clock_t start = clock();
    for (i = 0; i < ITER2; i++)
    {
        j = rand() % ((0 - 160 + 1) + 0);
        wx = 0.0;
        for (k = 0; k < list->wordCount; k++)
        {
            wx += w[k] * vector->vectorList[j][k];
        }
        grad = ((realData[j] - tanh(wx)));
        for (k = 0; k < list->wordCount; k++)
        {
            w[k] = w[k] - ((LR * grad * dtanh(wx) * vector->vectorList[j][k]) * (-2.0 / list->wordCount));
        }
        error += grad * grad;
        errors[i] = error / vector->vectorCount;
        error = 0;
    }
    clock_t end = clock();
    double execTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\nExecution time for SGD : %lf", execTime);
}

void ADAM(wordList *list, double *realData, hotVectors *vector, double *w)
{
    int i, j, k;
    double b = 0, vw = 0, vb = 0, sw = 0, sb = 0;
    double beta1 = 0.9, beta2 = 0.999, epsilon = 0.00000001;
    double grad = 1, y_hat = 0, diff;
    double wx;
    double errors[ITER3];
    double error = 0;
    clock_t start = clock();
    for (i = 0; i < ITER3; i++)
    {
        for (j = 0; j < vector->vectorCount; j++)
        {
            y_hat = 0;
            wx = 0;
            grad = 0;
            for (k = 0; k < list->wordCount; k++)
            {
                y_hat += w[k] * vector->vectorList[j][k];
            }
            wx = tanh(y_hat);
            diff = (wx - realData[j]);
            grad = 0;
            for (k = 0; k < list->wordCount; k++)
            {
                grad = (2.0 / list->wordCount) * (diff) * (dtanh(y_hat)) * (vector->vectorList[j][k]);
                sw = sw * beta2 + (1 - beta2) * grad * grad;
                vw = vw * beta1 + (1 - beta1) * grad;
                w[k] = w[k] - ((0.001 * vw) / (sqrt(sw + epsilon)));
            }
            error += diff * diff;
        }
        errors[i] = error / vector->vectorCount;
        error = 0;
    }
    clock_t end = clock();
    double execTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\nExecution time for ADAM : %lf", execTime);
}

void calculateResultFromMethods(double *w, hotVectors *testData, double *realData, int wordCount)
{
    int i, j, trueCount = 0, falseCount = 0;
    for (i = 0; i < testData->vectorCount; i++)
    {
        double result = 0.0;
        for (j = 0; j < wordCount; j++)
        {
            result += w[j] * testData->vectorList[i][j];
        }
        result = tanh(result);
        printf("\nReal value %.10lf | %.10lf Estimated value", realData[i], result);

        if (pow(realData[i] - result, 2) < 0.75)
        {
            trueCount++;
            printf("\n%dth sentence is estimated correctly!", i + 1);
            /*
            if (realData[i] < 0)
                printf("\nThis sentence belongs to Movie class.", i + 1);
            else
                printf("\nThis sentence belongs to Technology class. ", i + 1);
                */
        }
        else
        {
            // printf("\n%dth sentence is estimated incorrectly!", i + 1);
            falseCount++;
        }
    }
    printf("\nTrue count %d | %d False count", trueCount, falseCount);
}
void assignParameters(double *parameters, double assignValue, int len)
{
    int i;
    printf("\nAssigning value : %lf", assignValue);
    for (i = 0; i < len; i++)
        parameters[i] = assignValue;
}
void trainMethods(wordList *dictionary, double *realData, hotVectors *vectors, double ***initialParameters, double *initialValue)
{
    int i = 0, j = 0;
    for (i = 0; i < 5; i++)
    {
        printf("\nGiven Initial Value : %lf", initialValue[i]);
        gradientDescent(dictionary, realData, vectors, initialParameters[0][i]);
        stochasticGradientDescent(dictionary, realData, vectors, initialParameters[1][i]);
        ADAM(dictionary, realData, vectors, initialParameters[2][i]);
    }
}
void writeParameters(char *filename, double ***initialParameters, int wordCount)
{
    int i, j, k;
    FILE *fptr = fopen(filename, "w");
    if (!fptr)
    {
        perror("\nError! Cannot open file!");
        return;
    }

    for (i = 0; i < wordCount; i++)
    {
        fprintf(fptr, "%lf,", initialParameters[0][0][i]);
        fprintf(fptr, "%lf,", initialParameters[0][1][i]);
        fprintf(fptr, "%lf,", initialParameters[0][2][i]);
        fprintf(fptr, "%lf,", initialParameters[0][3][i]);
        fprintf(fptr, "%lf,", initialParameters[0][4][i]);
        fprintf(fptr, "%s,", "1");

        fprintf(fptr, "\n%s", "");
    }
    for (i = 0; i < wordCount; i++)
    {
        fprintf(fptr, "%lf,", initialParameters[1][0][i]);
        fprintf(fptr, "%lf,", initialParameters[1][1][i]);
        fprintf(fptr, "%lf,", initialParameters[1][2][i]);
        fprintf(fptr, "%lf,", initialParameters[1][3][i]);
        fprintf(fptr, "%lf,", initialParameters[1][4][i]);
        fprintf(fptr, "%s,", "2");

        fprintf(fptr, "\n%s", "");
    }
    for (i = 0; i < wordCount; i++)
    {
        fprintf(fptr, "%lf,", initialParameters[2][0][i]);
        fprintf(fptr, "%lf,", initialParameters[2][1][i]);
        fprintf(fptr, "%lf,", initialParameters[2][2][i]);
        fprintf(fptr, "%lf,", initialParameters[2][3][i]);
        fprintf(fptr, "%lf,", initialParameters[2][4][i]);
        fprintf(fptr, "%s,", "3");

        fprintf(fptr, "\n%s", "");
    }
    printf("\nDatas are written in %s successfully.", filename);
    fclose(fptr);
}
void writeErrors(double errors[], int iter)
{
    int i, j;
    FILE *fptr = fopen("errorGD.csv", "w");
    if (!fptr)
    {
        perror("\nError! Cannot open file!");
        return;
    }
    fprintf(fptr, "%s\n", "");
    for (i = 0; i < iter; i++)
    {
        fprintf(fptr, "%lf,", errors[i]);
        fprintf(fptr, "%s\n", "");
    }
    fprintf(fptr, "%s\n", "ITEREND");
    fclose(fptr);
    printf("\nErrors has been written successfully.");
}