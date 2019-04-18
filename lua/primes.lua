import = math
function is_prime(n)
    local m = math.ceil(math.sqrt(n))
    for i = 2,m do
        if (n % i == 0) and (n > i) then
            return false
        end
    end
    return true
end            
