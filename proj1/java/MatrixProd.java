import java.util.Arrays;

class MatrixProd {
    private static enum Algorithm {
        REGULAR, 
        LINE_BY_LINE,
    }

    public static void main(String[] args) {
        if (args.length < 2) {
            printUsage();
            System.exit(1);
        }

        int algorithm = Integer.parseInt(args[0]);
        int mat_size = Integer.parseInt(args[1]);

        if (algorithm < 1 || algorithm > Algorithm.values().length) {
            System.out.println("Invalid algorithm!");
            printUsage();
            System.exit(2);
        }

        String exec_time = multiplyMatrix(mat_size, algorithm == 1 ? Algorithm.REGULAR : Algorithm.LINE_BY_LINE);
        System.out.println(String.format("%s N/A N/A", exec_time));
    }

    private static String multiplyMatrix(int mat_size, Algorithm algorithm) {
        double[] mat1 = new double[mat_size * mat_size];
        double[] mat2 = new double[mat_size * mat_size];
        double[] mat_res = new double[mat_size * mat_size];

        Arrays.fill(mat1, 1.0);
        for (int i = 0; i < mat_size; ++i) {
            Arrays.fill(mat2, mat_size * i, mat_size * (i + 1), 1.0 + i);
        }
        Arrays.fill(mat_res, 0.0);

        long start_time = System.nanoTime();

        if (algorithm == Algorithm.LINE_BY_LINE) {
            for (int i = 0; i < mat_size; ++i) {
                for (int k = 0; k < mat_size; ++k) {
                    for (int j = 0; j < mat_size; ++j) {
                        mat_res[i*mat_size + j] += mat1[i*mat_size + k] * mat2[k*mat_size + j];
                    }
                }
            }
        } else {
            for (int i = 0; i < mat_size; ++i) {
                for (int j = 0; j < mat_size; ++j) {
                    double acc = 0;
                    for (int k = 0; k < mat_size; ++k) {
                        acc += mat1[i*mat_size + k] * mat2[k*mat_size + j];
                    }
                    mat_res[i*mat_size + j] = acc;
                }
            }
        }

        long end_time = System.nanoTime();

        return String.format("%.3f", (double)((end_time - start_time)/1e9));
    }

    private static void printUsage() {
        System.out.println("usage: MatrixProd <algorith> <matrix size>");
        System.out.println("algorithms:");
        System.out.println("\t1: regular");
        System.out.println("\t2: line by line");
    }
}