#include <vector>

template <typename T>
class Ringbuffer : protected std::vector<T> {

    protected:

        typedef std::vector<T> parent; // alias for parent class
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type&& move_reference;
        int32_t front_index; // index of the front element

    public:

        // empty constructor
        Ringbuffer(): parent(0), front_index(0) {}

        // default constructor
        Ringbuffer(const int32_t ringbuffer_size): parent(ringbuffer_size), front_index(0) {}

        // fill constructor
        Ringbuffer(const int32_t ringbuffer_size, const_reference val): parent(ringbuffer_size, val), front_index(0) {}

        // range constructor
        template <class InputIterator>
        Ringbuffer (InputIterator first, InputIterator last): parent(first, last), front_index(0) {}

        // initilizer list constructor
        Ringbuffer(std::initializer_list<value_type> il): parent(il), front_index(0) {}

        // copy constructor
        Ringbuffer(const Ringbuffer& other): parent(other), front_index(other.front_index) {}

        // move constructor
        Ringbuffer(Ringbuffer&& other) : parent(std::move(other)), front_index(other.front_index) {}

        // destructor
        virtual ~Ringbuffer() {}

        // copy assignment operator
        void operator=(const Ringbuffer& other) {
            if (this != &other) {
                this->parent::operator=(other);
                this->front_index = other.front_index;
            }
        }

        // move assignment operator
        void operator=(Ringbuffer&& other) {
            if (this != &other) {
                this->parent::operator=(std::move(other));
                this->front_index = other.front_index;
            }
        }


        // size
        int32_t size() const { return this->parent::size(); }

        // index substitution
        int32_t ring_idx_to_vec_idx(const int32_t index) const {
            if (this->size() > 0) {
                return (this->front_index + index + this->size()) % this->size();
            }
            else {
                return 0;
            }
        }

        // change front index
        int32_t change_front_index(const int32_t vec_index) {
            this->front_index = this->ring_idx_to_vec_idx(vec_index);
            return front_index;
        }

        // decrement front index
        int32_t decrement_front_index(int32_t const amount = 1) {
            if (this->size() > 0) {
                this->front_index -= amount;
                while (this->front_index < 0) { this->front_index += this->size(); } // % (Modulo doesn't work if integer is negative)
            }
            else {
                this->front_index = 0;
            }
            return this->front_index;
        }

        // increment front index
        int32_t increment_front_index(int32_t const amount = 1) {
            if (this->size() > 0) {
                this->front_index += amount;
                this->front_index %= this->size();
                return front_index;
            }
            else {
                this->front_index = 0;
            }
            return this->front_index;
        }

        // Index operator
        reference at(const int32_t index) {
            return this->parent::operator[](this->ring_idx_to_vec_idx(index)); // a negative index is possible down to <(-1) * this->size()>
        }

        // const Index operator
        const_reference at(const int32_t index) const {
            return this->parent::operator[](this->ring_idx_to_vec_idx(index)); // a negative index is possible down to <(-1) * this->size()>
        }

        // Index operator
        reference operator[] (const int32_t index) {
            return this->at(index);
        }

        // const Index operator
        const_reference operator[] (const int32_t index) const {
            return this->at(index);
        }


        class iterator {

            public:

                Ringbuffer* buffer;
                int32_t index;

            public:

                // constuctor
                iterator(Ringbuffer* const buf, const int32_t idx): buffer(buf), index(idx) {}

                // copy constructor
                iterator(const iterator& other): buffer(other.buffer), index(other.index) {}

                // copy assignment operator
                void operator=(const iterator& other) {
                    if(this != &other) {
                        this->buffer = other.buffer;
                        this->index = other.index;
                    }
                }

                // dereference operator
                reference operator*() const {
                    return this->buffer->at(index);
                }

                // dereference operator
                pointer operator->() const {
                    return &(this->buffer->at(index));
                }

                // prefix ++
                iterator operator++() {
                    this->index += 1;
                    return *this;
                }

                // postfix ++
                iterator operator++(int) {
                    iterator other = *this;
                    other.index += 1;
                    return other;
                }

                // prefix --
                iterator operator--() {
                    this->index -= 1;
                    return *this;
                }

                // postfix --
                iterator operator--(int) {
                    iterator other = *this;
                    other.index -= 1;
                    return other;
                }

                // operator +=
                void operator+=(int32_t offset) {
                    this->index += offset;
                }

                // operator +
                iterator operator+(int32_t offset) const {
                    iterator other = *this;
                    other.index += offset;
                    return other;
                }

                // operator -=
                void operator-=(int32_t offset) {
                    this->index -= offset;
                }

                // operator -
                iterator operator-(int32_t offset) const {
                    iterator other = *this;
                    other.index -= offset;
                    return other;
                }

                // operator -
                int32_t operator-(const iterator& other) const {
                    if (this == &other) {
                        return this->index - other.index;
                    }
                    else {
                        return 0;
                    }
                }

                // comparison operator ==
                bool operator==(const iterator& other) const {
                    return (this->index == other.index);
                }

                // comparison operator !=
                bool operator!=(const iterator& other) const {
                    return !(*this == other);
                }

        };

        class const_iterator {

            public:

                const Ringbuffer* buffer;
                int32_t index;

            public:

                // constuctor
                const_iterator(const Ringbuffer* const buf, const int32_t idx): buffer(buf), index(idx) {}

                // copy constructor
                const_iterator(const iterator& other): buffer(other.buffer), index(other.index) {}

                // copy constructor
                const_iterator(const const_iterator& other): buffer(other.buffer), index(other.index) {}

                // copy assignment operator
                void operator=(const iterator& other) {
                    if(this != &other) {
                        this->buffer = other.buffer;
                        this->index = other.index;
                    }
                }

                // copy assignment operator
                void operator=(const const_iterator& other) {
                    if(this != &other) {
                        this->buffer = other.buffer;
                        this->index = other.index;
                    }
                }

                // dereference operator
                const_reference operator*() const {
                    return this->buffer->at(index);
                }

                // dereference operator
                const_pointer operator->() const {
                    return &(this->buffer->at(index));
                }

                // prefix ++
                const_iterator operator++() {
                    this->index += 1;
                    return *this;
                }

                // postfix ++
                const_iterator operator++(int) {
                    const_iterator other = *this;
                    other.index += 1;
                    return other;
                }

                // prefix --
                const_iterator operator--() {
                    this->index -= 1;
                    return *this;
                }

                // postfix --
                const_iterator operator--(int) {
                    const_iterator other = *this;
                    other.index -= 1;
                    return other;
                }

                // operator +=
                void operator+=(int32_t offset) {
                    this->index += offset;
                }

                // operator +
                const_iterator operator+(int32_t offset) const {
                    const_iterator other = *this;
                    other.index += offset;
                    return other;
                }

                // operator -=
                void operator-=(int32_t offset) {
                    this->index -= offset;
                }

                // operator -
                const_iterator operator-(int32_t offset) const {
                    const_iterator other = *this;
                    other.index -= offset;
                    return other;
                }

                // operator -
                int32_t operator-(const const_iterator& other) const {
                    if (this == &other) {
                        return this->index - other.index;
                    }
                    else {
                        return 0;
                    }
                }


                // comparison operator ==
                bool operator==(const const_iterator& other) const {
                    return (this->index == other.index);
                }

                // comparison operator !=
                bool operator!=(const const_iterator& other) const {
                    return !(*this == other);
                }


        };

        class reverse_iterator {

            public:

                Ringbuffer* buffer;
                int32_t index;

            public:

                // constuctor
                reverse_iterator(Ringbuffer* const buf, const int32_t idx): buffer(buf), index(idx) {}

                // copy constructor
                reverse_iterator(const reverse_iterator& other): buffer(other.buffer), index(other.index) {}

                // copy assignment operator
                void operator=(const reverse_iterator& other) {
                    if(this != &other) {
                        this->buffer = other.buffer;
                        this->index = other.index;
                    }
                }

                // dereference operator
                reference operator*() const {
                    return this->buffer->at(index);
                }

                // dereference operator
                pointer operator->() const {
                    return &(this->buffer->at(index));
                }

                // prefix ++
                reverse_iterator operator++() {
                    this->index -= 1;
                    return *this;
                }

                // postfix ++
                reverse_iterator operator++(int) {
                    reverse_iterator other = *this;
                    other.index -= 1;
                    return other;
                }

                // prefix --
                reverse_iterator operator--() {
                    this->index += 1;
                    return *this;
                }

                // postfix --
                reverse_iterator operator--(int) {
                    reverse_iterator other = *this;
                    other.index += 1;
                    return other;
                }

                // operator +=
                void operator+=(int32_t offset) {
                    this->index -= offset;
                }

                // operator +
                reverse_iterator operator+(int32_t offset) const {
                    reverse_iterator other = *this;
                    other.index -= offset;
                    return other;
                }

                // operator -=
                void operator-=(int32_t offset) {
                    this->index += offset;
                }

                // operator -
                reverse_iterator operator-(int32_t offset) const {
                    reverse_iterator other = *this;
                    other.index += offset;
                    return other;
                }

                // operator -
                int32_t operator-(const reverse_iterator& other) const {
                    if (this == &other) {
                        return other.index - this->index;
                    }
                    else {
                        return 0;
                    }
                }

                // comparison operator ==
                bool operator==(const reverse_iterator& other) const {
                    return (this->index == other.index);
                }

                // comparison operator !=
                bool operator!=(const reverse_iterator& other) const {
                    return !(*this == other);
                }

        };

        class const_reverse_iterator {

            public:

                const Ringbuffer* buffer;
                int32_t index;

            public:

                // constuctor
                const_reverse_iterator(const Ringbuffer* const buf, const int32_t idx): buffer(buf), index(idx) {}

                // copy constructor
                const_reverse_iterator(const reverse_iterator& other): buffer(other.buffer), index(other.index) {}

                // copy constructor
                const_reverse_iterator(const const_reverse_iterator& other): buffer(other.buffer), index(other.index) {}

                // copy assignment operator
                void operator=(const reverse_iterator& other) {
                    if(this != &other) {
                        this->buffer = other.buffer;
                        this->index = other.index;
                    }
                }

                // copy assignment operator
                void operator=(const const_reverse_iterator& other) {
                    if(this != &other) {
                        this->buffer = other.buffer;
                        this->index = other.index;
                    }
                }

                // dereference operator
                const_reference operator*() const {
                    return this->buffer->at(index);
                }

                // dereference operator
                const_pointer operator->() const {
                    return &(this->buffer->at(index));
                }

                // prefix ++
                const_reverse_iterator operator++() {
                    this->index -= 1;
                    return *this;
                }

                // postfix ++
                const_reverse_iterator operator++(int) {
                    const_reverse_iterator other = *this;
                    other.index -= 1;
                    return other;
                }

                // prefix --
                const_reverse_iterator operator--() {
                    this->index += 1;
                    return *this;
                }

                // postfix --
                const_reverse_iterator operator--(int) {
                    const_reverse_iterator other = *this;
                    other.index += 1;
                    return other;
                }

                // operator +=
                void operator+=(int32_t offset) {
                    this->index -= offset;
                }

                // operator +
                const_reverse_iterator operator+(int32_t offset) const {
                    const_reverse_iterator other = *this;
                    other.index -= offset;
                    return other;
                }

                // operator -=
                void operator-=(int32_t offset) {
                    this->index += offset;
                }

                // operator -
                const_reverse_iterator operator-(int32_t offset) const {
                    const_reverse_iterator other = *this;
                    other.index += offset;
                    return other;
                }

                // operator -
                int32_t operator-(const const_reverse_iterator& other) const {
                    if (this == &other) {
                        return other.index - this->index;
                    }
                    else {
                        return 0;
                    }
                }

                // comparison operator ==
                bool operator==(const const_reverse_iterator& other) const {
                    return (this->index == other.index);
                }

                // comparison operator !=
                bool operator!=(const const_reverse_iterator& other) const {
                    return !(*this == other);
                }

        };


        // iterator begin
        iterator begin() {
            return iterator(this, 0);
        }

        // iterator begin
        const_iterator begin() const {
            return const_iterator(this, 0);
        }

        // iterator end
        iterator end() {
            return iterator(this, this->size());
        }

        // iterator end
        const_iterator end() const {
            return const_iterator(this, this->size());
        }


        // iterator begin
        reverse_iterator rbegin() {
            return reverse_iterator(this, this->size()-1);
        }

        // iterator begin
        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(this, this->size()-1);
        }

        // iterator end
        reverse_iterator rend() {
            return reverse_iterator(this, -1);
        }

        // iterator end
        const_reverse_iterator rend() const {
            return const_reverse_iterator(this, -1);
        }


        // iterator begin
        const_iterator cbegin() const {
            return this->begin();
        }

        // iterator end
        const_iterator cend() const {
            return this->end();
        }

        // iterator begin
        const_reverse_iterator crbegin() const {
            return this->rbegin();
        }

        // iterator end
        const_reverse_iterator crend() const {
            return this->rend();
        }


        // Access first element
        reference front() {
            return *(this->begin());
        }

        // Access first element
        const_reference front() const {
            return *(this->begin());
        }

        // Access last element
        reference back() {
            return *(this->rbegin());
        }

        // Access last element
        const_reference back() const {
            return *(this->rbegin());
        }



        // add (copy) element to buffer
        void push_front(const_reference val) {

            // decrement front index
            this->decrement_front_index();

            // assign (copy) element
            this->front() = val;
        }

        // add (move) element to buffer
        void push_front(move_reference val) {

            // decrement front index
            this->decrement_front_index();

            // assign (move) element
            this->front() = std::move(val);
        }

        // += operator (copy)
        void operator+= (const_reference val) {
            return this->push_front(val);
        }

        // += operator (move)
        void operator+= (move_reference val) {
            return this->push_front(val);
        }


        // add (copy) element to buffer
        void push_back(const_reference val) {

            // increment front index
            this->increment_front_index();

            // assign (copy) element
            this->back() = val;

        }

        // add (move) element to buffer
        void push_back(move_reference val) {


            // increment front index
            this->increment_front_index();

            // assign (copy) element
            this->back() = std::move(val);
        }


        // pop element from buffer
        value_type pop_front() {

            // assign (copy) element
            value_type val = this->front();
            
            this->erase(this->begin());

            return val;
        }

        // pop element from buffer
        value_type pop_back() {

            // assign (copy) element
            value_type val = this->back();
            
            this->erase(this->end()-1);

            return val;
        }


        /* The container is extended by inserting a new element at position. 
         * This new element is constructed in place using args as the arguments for its construction.
         */
        template <class... Args>
        iterator emplace(const_iterator position, Args&&... args) {

            // Get position & iterator for underlying vector
            auto old_end = this->cend();
            const int32_t vec_index = this->ring_idx_to_vec_idx(position.index);
            auto vec_pos_it = this->parent::begin() + vec_index;

            // construct element
            this->parent::emplace(vec_pos_it, args...);

            // update front index
            if (this->front_index > vec_index || position == old_end) {
                // increment front index
                this->increment_front_index();
            }

            // return iterator to new element
            return iterator(this, position.index);
        }

        /* Inserts a new element at the beginning of the container, right before its current first element. 
         * This new element is constructed in place using args as the arguments for its constructor.
         */
        template <class... Args>
        iterator emplace_front(Args&&... args) {
            return this->emplace(this->begin(), args...);
        }

        /* Inserts a new element at the end of the container, right after its current last element. 
         * This new element is constructed in place using args as the arguments for its constructor.
         */
        template <class... Args>
        iterator emplace_back(Args&&... args) {
            return this->emplace(this->end(), args...);
        }


        // single element (1)	
        iterator insert (const_iterator position, const value_type& val) {

            // Get position & iterator for underlying vector
            auto old_end = this->cend();
            const int32_t vec_index = this->ring_idx_to_vec_idx(position.index);
            auto vec_pos_it = this->parent::begin() + vec_index;

            // insert element
            this->parent::insert(vec_pos_it, val);

            // update front index
            if (this->front_index > vec_index || position == old_end) {
                // increment front index
                this->increment_front_index();
            }

            // return iterator to new element
            return iterator(this, position.index);
        }

        // fill (2)	
        void insert (const_iterator position, int32_t n, const value_type& val) {

            // Get position & iterator for underlying vector
            auto old_end = this->cend();
            const int32_t vec_index = this->ring_idx_to_vec_idx(position.index);
            auto vec_pos_it = this->parent::begin() + vec_index;

            // insert element
            this->parent::insert(vec_pos_it, n, val);

            // update front index
            if (this->front_index > vec_index || position == old_end) {
                // increment front index
                this->increment_front_index(n);
            }

            // // return iterator to new element
            // return iterator(this, position.index);
        }

        // range (3)	
        template <class RandomAccessIterator>
        void insert (const_iterator position, RandomAccessIterator first, RandomAccessIterator last) {

            // Get position & iterator for underlying vector
            auto old_end = this->cend();
            const int32_t vec_index = this->ring_idx_to_vec_idx(position.index);
            auto vec_pos_it = this->parent::begin() + vec_index;
            int32_t n = (last - first);

            // insert element
            this->parent::insert(vec_pos_it, first, last);

            // update front index
            if (this->front_index > vec_index || position == old_end) {
                // increment front index
                this->increment_front_index(n);
            }

            // // return iterator to new element
            // return iterator(this, position.index);
        }

        /* Removes from the vector a single element (position).
         * This effectively reduces the container size by the number of elements removed, which are destroyed.
         */
        iterator erase(const_iterator position) {

            // Get position & iterator for underlying vector
            const int32_t vec_index = this->ring_idx_to_vec_idx(position.index);
            auto vec_pos_it = this->parent::begin() + vec_index;

            // erase element
            this->parent::erase(vec_pos_it);

            // update front index
            if (this->front_index > vec_index) {
                // decrement front index
                this->decrement_front_index();
            }

            // return iterator to new element
            return iterator(this, position.index);
        }

        /* Removes from the vector a range of elements ([first,last)).
         * This effectively reduces the container size by the number of elements removed, which are destroyed.
         */
        iterator erase (const_iterator first, const_iterator last) {

            for (auto it = first; it != last; ++it) {
                this->erase(first); // second element moves to position of the first element after that the first element was erased.
            }

            return iterator(this, first.index);
        }

        // Clear container content
        void clear() {
            this->front_index = 0;
            this->parent::clear();
        }

};