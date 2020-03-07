const util = require('util');
const exec = util.promisify(require('child_process').exec);
const fs = require('fs');
const EXECUTABLE_PATH = "cpp/matrixprod.out";

const printUsage = () => {
    console.log(`usage: ${process.argv[1]} <num trials> <initial size> <last size> <increment size> <output filename>`);
}

if (process.argv.length < 7) {
    printUsage();
    process.exit(1);
}

const [num_trials, initial_size, last_size, increment_size] = process.argv.slice(2, 6).map(num => parseInt(num, 10));
const output_filename = process.argv[6];

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

const getAverage = (list) => (list.reduce((prev, curr) => prev + curr, 0) / list.length).toFixed(3);

const getResultsCSV = async () => {
    const csv = [];

    for (let matrix_size = initial_size; matrix_size <= last_size; matrix_size += increment_size) {
        for (let algorithm = 1; algorithm <= 2; ++algorithm) {
            const exec_times = [];
            const l1_cache_misses = [];
            const l2_cache_misses = [];
    
            for (let i = 0; i < num_trials; ++i) {
                const { stdout } = await exec(`${EXECUTABLE_PATH} ${algorithm} ${matrix_size}`);
                const [time, l1cm, l2cm] = stdout.split(" ");
                exec_times.push(parseFloat(time));
                l1_cache_misses.push(parseInt(l1cm, 10));
                l2_cache_misses.push(parseInt(l2cm, 10));
            }
    
            const exec_time_avg = getAverage(exec_times);
            const l1_cache_misses_avg = getAverage(l1_cache_misses);
            const l2_cache_misses_avg = getAverage(l2_cache_misses);
            
            const csv_line = `${matrix_size},${algorithm},${exec_time_avg},${l1_cache_misses_avg},${l2_cache_misses_avg}`;
            console.log(csv_line);
            csv.push(csv_line);
        }
    }

    return csv.join("\n");
}

getResultsCSV()
    .then((csv) => {
        fs.writeFileSync(output_filename, csv);
        console.log("Done.");
    })
    .catch((err) => {
        console.error(err);
        process.exit(4);
    });
