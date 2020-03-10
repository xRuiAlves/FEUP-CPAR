const util = require('util');
const exec = util.promisify(require('child_process').exec);
const fs = require('fs');

const LANGUAGES = Object.freeze({
    1: "cpp",
    2: "python",
});
const EXECUTABLES = Object.freeze({
    "cpp": "cpp/matrixprod.out",
    "python": "pypy3 python/matrixprod.py",
});
const NUM_LANGUAGES = LANGUAGES.length;
const NUM_ALGORITHMS = 3;

const printUsage = () => {
    console.log(`usage: ${process.argv[1]} <languages> <algorithms> <num trials> <initial size> <last size> <increment size> <output filename>`);
    console.log("languages:");
    console.log("\t1: cpp");
    console.log("\t2: python");
    console.log("algorithms:");
    console.log("\t1: regular");
    console.log("\t2: line by line");
    console.log("\t3: clustering");
    console.log("\nExample (running alg. 1 and 2 in cpp and python):");
    console.log("./results_script 1,2 1,2 5 200 1000 100 output.csv");
}

const parseList = (list_str) => list_str.split(",").map((elem) => parseInt(elem, 10)).sort();

if (process.argv.length < 9) {
    printUsage();
    process.exit(1);
}

const [languages, algorithms] = process.argv.slice(2, 4);
const [num_trials, initial_size, last_size, increment_size] = process.argv.slice(4, 8).map(num => parseInt(num, 10));
const output_filename = process.argv[8];

const language_list = parseList(languages);
const algorithm_list = parseList(algorithms);

if (initial_size <= 0 || last_size <= 0 || increment_size <= 0) {
    console.log("Matrix sizes must be positive!");
    printUsage();
    process.exit(4);
}

if (!(initial_size < last_size)) {
    console.log("Matrix initial size must be smaller than the matrix last size!");
    printUsage();
    process.exit(5);
}

language_list.forEach((language) => {
    if (language < 1 || language > NUM_LANGUAGES) {
        console.log(`Language ${language} is invalid!`);
        printUsage();
        process.exit(6);
    }
})

algorithm_list.forEach((algorithm) => {
    if (algorithm < 1 || algorithm > NUM_ALGORITHMS) {
        console.log(`Algorithm ${algorithm} is invalid!`);
        printUsage();
        process.exit(7);
    }
})

const getAverage = (list) => (list.reduce((prev, curr) => prev + curr, 0) / list.length).toFixed(3);

const addHeaderToCSV = () => {
    if (!fs.existsSync(output_filename)) {
        const csv_header = "m_size,language,algorithm,time,l1_cm,l2_cm\n";
        fs.appendFileSync(output_filename, csv_header);
    }
}

const getResultsCSV = async () => {
    addHeaderToCSV();
    
    for (let matrix_size = initial_size; matrix_size <= last_size; matrix_size += increment_size) {
        for (const language of language_list) {
            const exec_path = EXECUTABLES[LANGUAGES[language]];
            for (const algorithm of algorithm_list) {
                const exec_times = [];
                const l1_cache_misses = [];
                const l2_cache_misses = [];
        
                for (let i = 0; i < num_trials; ++i) {
                    const { stdout } = await exec(`${exec_path} ${algorithm} ${matrix_size}`);
                    const [time, l1cm, l2cm] = stdout.split(" ");
                    exec_times.push(parseFloat(time));
                    l1_cache_misses.push(parseInt(l1cm, 10));
                    l2_cache_misses.push(parseInt(l2cm, 10));
                }
        
                const exec_time_avg = getAverage(exec_times);
                const l1_cache_misses_avg = getAverage(l1_cache_misses);
                const l2_cache_misses_avg = getAverage(l2_cache_misses);
                
                const csv_line = `${matrix_size},${LANGUAGES[language]},${algorithm},${exec_time_avg},${l1_cache_misses_avg},${l2_cache_misses_avg}`;
                console.log(csv_line);
                fs.appendFileSync(output_filename, `${csv_line}\n`);
            }
        }
    }
}

getResultsCSV()
    .then(() => {
        console.log("Done.");
    })
    .catch((err) => {
        console.error(err);
        process.exit(8);
    });
