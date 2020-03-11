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
const NUM_LANGUAGES = Object.keys(LANGUAGES).length;
const NUM_ALGORITHMS = 3;

const printUsage = () => {
    console.log(`usage: ${process.argv[1]} <languages> <algorithms> <num trials> <initial size> <last size> <increment size> <output filename> [<block sizes>]`);
    console.log("languages:");
    console.log("\t1: cpp");
    console.log("\t2: python");
    console.log("algorithms:");
    console.log("\t1: regular");
    console.log("\t2: line by line");
    console.log("\t3: block by block (requires block sizes input)");
    console.log("\nExample (running alg. 1 and 2 in cpp and python):");
    console.log("./experiments.js 1,2 1,2 5 200 1000 100 output1.csv 25,50,100");
    console.log("./experiments.js 1 3 10 1024 2048 256 output2.csv 64,128,256");
}

const parseList = (list_str) => list_str.split(",").map((elem) => parseInt(elem, 10)).sort((n1, n2) => n1 - n2);

if (process.argv.length < 9) {
    printUsage();
    process.exit(1);
}

const [languages, algorithms] = process.argv.slice(2, 4);
const [num_trials, initial_size, last_size, increment_size] = process.argv.slice(4, 8).map(num => parseInt(num, 10));
const output_filename = process.argv[8];
const block_sizes = process.argv[9] || "0";

const language_list = parseList(languages);
const algorithm_list = parseList(algorithms);
const block_size_list = parseList(block_sizes);

if (algorithm_list.includes(3) && process.argv.length < 10) {
    console.log("For algorithm 3, block sizes must be specified!")
    printUsage();
    process.exit(1);
}

if (algorithm_list.includes(3) && algorithm_list.length !== 1) {
    console.log("Please run algorithm 3 separately from other algorithms!")
    printUsage();
    process.exit(1);
}

if (algorithm_list.includes(3) && language_list.includes(2)) {
    console.log("For algorithm 3, only cpp is allowed!")
    printUsage();
    process.exit(1);
}

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

const addHeaderToCSV = () => {
    if (!fs.existsSync(output_filename)) {
        const csv_header = `m_size,language,algorithm${algorithm_list.includes(3) ? ",block_size" : ""},time,l1_cm,l2_cm\n`;
        fs.appendFileSync(output_filename, csv_header);
    }
}

const getResultsCSV = async () => {
    addHeaderToCSV();
    
    for (let matrix_size = initial_size; matrix_size <= last_size; matrix_size += increment_size) {
        for (const language of language_list) {
            const exec_path = EXECUTABLES[LANGUAGES[language]];
            for (const algorithm of algorithm_list) {
                for (const block_size of block_size_list) {
                    let exec_time_avg = 0;
                    let l1_cache_misses_avg = 0;
                    let l2_cache_misses_avg = 0;
            
                    for (let i = 0; i < num_trials; ++i) {
                        const { stdout } = await exec(`${exec_path} ${algorithm} ${matrix_size} ${block_size}`);
                        const [time, l1cm, l2cm] = stdout.split(" ");
                        exec_time_avg += parseFloat(time);
                        l1_cache_misses_avg += parseInt(l1cm, 10);
                        l2_cache_misses_avg += parseInt(l2cm, 10);
                    }

                    exec_time_avg = (exec_time_avg / num_trials).toFixed(3);
                    l1_cache_misses_avg = (l1_cache_misses_avg / num_trials).toFixed(3);
                    l2_cache_misses_avg = (l2_cache_misses_avg / num_trials).toFixed(3);
                    
                    const block_size_str = algorithm === 3 ? `,${block_size}` : "";
                    const csv_line = `${matrix_size},${LANGUAGES[language]},${algorithm}${block_size_str},${exec_time_avg},${l1_cache_misses_avg},${l2_cache_misses_avg}`;
                    console.log(csv_line);
                    fs.appendFileSync(output_filename, `${csv_line}\n`);
                }
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
