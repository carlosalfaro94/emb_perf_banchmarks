function is_prime(n::Integer)
    m = ceil(sqrt(n))
    for i in 2:m
        if n % i == 0 && n != i
            return false
        end
    end
    return true
end 