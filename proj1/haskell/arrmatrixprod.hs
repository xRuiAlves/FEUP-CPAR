-- For getArgs
import System.Environment
-- For arrays
import Data.Array
-- For deepseq forcing deep evaluation of result matrix
import Control.DeepSeq
-- For counting execution time
import System.CPUTime

-- https://stackoverflow.com/questions/31239163/print-each-value-in-list-n-times
repeatListInline :: Int -> [a] -> [a]
replicate' n x = take n (repeat x)
repeatListInline _ [] = []
repeatListInline n (x:xs) = (replicate' n x) ++ repeatListInline n xs

-- genMat1 :: (Ix i, Fractional a) => Int -> Array (i) a
genMat1 :: (Num i, Ix i) => i -> Array i Double
genMat1 size = listArray (0, size*size - 1) (repeat 1.0)

-- genMat2 :: (Ix i, Fractional a) => Int -> Array (i) a
genMat2 :: Int -> Array Int Double
genMat2 size = listArray (0, size*size - 1) (repeatListInline size [fromIntegral i + 1.0 | i <- [0..size-1]])

-- getMatCol :: (Ix i, Fractional a) => Int -> Array (i) a -> Int -> [a]
getMatCol :: (Num i, Ix i, Enum i) => i -> Array (i) a -> i -> [a]
getMatCol col mat size 
    | col < size = [ mat!(col + i*size) | i <- [0..size-1] ]
    | otherwise = error "Column index out of range!"

-- getMatLine :: (Ix i, Fractional a) => Int -> Array (i) a -> Int -> [a]
getMatLine :: (Num i, Ix i, Enum i) => i -> Array (i) a -> i -> [a]
getMatLine line mat size
    | line < size = [ mat!(i + line*size) | i <- [0..size-1] ]
    | otherwise = error "Line index out of range!"

dotProduct :: Num a => [a] -> [a] -> a
dotProduct a b | length a == length b = sum (zipWith (*) a b)
               | otherwise = error "Vector sizes must match"

-- matrixMult :: (Ix i, Fractional a) => Array (i) a -> Array (i) a -> Int -> [a]
matrixMult :: (Enum i, Num i, Ix i, Num a) => Array i a -> Array i a -> i -> [a]
matrixMult mat1 mat2 size = 
    [dotProduct (getMatLine i mat1 size) (getMatCol j mat2 size) | i <- [0..size-1], j <- [0..size-1]]

printUsage = print "Usage: matrixprod <size>"

parseArgs [] = error "size must be provided!"
parseArgs [size_str] = 
    read size_str :: Int
parseArgs _ = error "too many arguments!"

main = do
    args <- getArgs
    let size = parseArgs args
    let mat1 = genMat1 size
        mat2 = genMat2 size
        result = matrixMult mat1 mat2 size
    
    start <- getCPUTime


    end <- result `deepseq` getCPUTime

    print "Time: "
    print ((fromIntegral (end - start)) / 10^12)

    -- print [ result!i | i <- [0..(min (size-1) 10)] ]
    -- print (take (min 10 size) result)
    -- print result
