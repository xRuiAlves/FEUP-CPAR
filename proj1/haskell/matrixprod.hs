import System.Environment

-- https://stackoverflow.com/questions/31239163/print-each-value-in-list-n-times
repeatListInline :: Int -> [a] -> [a]
replicate' n x = take n (repeat x)
repeatListInline _ [] = []
repeatListInline n (x:xs) = (replicate' n x) ++ repeatListInline n xs

genMat1 :: Fractional a => Int -> [a]
genMat1 size = repeatListInline (floor $ (fromIntegral size)**2) [1.0]

genMat2 :: Fractional a => Int -> [a]
genMat2 size = repeatListInline size [fromIntegral i + 1.0 | i <- [0..size-1]]

getMatCol :: Fractional a => Int -> [a] -> Int -> [a]
getMatCol col mat size 
    | col < size = [ mat!!(col + i*size) | i <- [0..size-1] ]
    | otherwise = error "Column index out of range!"

getMatLine :: Fractional a => Int -> [a] -> Int -> [a]
getMatLine line mat size
    | line < size = take size (drop (size*line) mat)
    | otherwise = error "Line index out of range!"

dotProduct :: Num a => [a] -> [a] -> a
dotProduct a b | length a == length b = sum (zipWith (*) a b)
               | otherwise = error "Vector sizes must match"

matrixMult :: Fractional a => [a] -> [a] -> Int -> [a]
matrixMult mat1 mat2 size = 
    [dotProduct (getMatLine i mat1 size) (getMatCol j mat2 size) | i <- [0..size-1], j <- [0..size-1]]

printUsage = print "Usage: matrixprod <size>"

parseArgs [] = error "size must be provided!"
parseArgs [size_str] = 
    let mat1 = genMat1 size
        mat2 = genMat2 size
        result = matrixMult mat1 mat2 size
    in
    print (take (min 10 size) result)
    where size = read size_str :: Int
parseArgs _ = error "too many arguments!"

main = do
    args <- getArgs
    parseArgs args
