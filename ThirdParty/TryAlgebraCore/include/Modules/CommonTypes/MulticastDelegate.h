// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <map>
#include <memory>


template<typename... Args>
class FTAMulticastDelegate {
public:
    typedef void(*TLam)(void*,void*, Args...);
    class Hndl
    {
    public:
        ~Hndl()
        {
            if (!Delegate.expired())
            {
                Delegate.lock()->Remove(Handle);
            }
        }
        Hndl(const std::weak_ptr<FTAMulticastDelegate<Args...>>& Del, int InHndl): Delegate(Del), Handle(InHndl)
        {}
    private:
        std::weak_ptr<FTAMulticastDelegate<Args...>> Delegate;
        int Handle = 0;
    };
    typedef std::shared_ptr<Hndl> HndlPtr;
private:
    struct FCallable
    {
        ~FCallable()
        {
            Deleter(Func_Obj);
        }
        void(*Deleter)(void*);
        void* Func_Obj = nullptr;
        void* Obj_void = nullptr;
        TLam Func;
        void Call(Args... args)
        {
            Func(Func_Obj,Obj_void,args...);
        }
    };
public:
    FTAMulticastDelegate()
    {
        SelfShared = std::shared_ptr<FTAMulticastDelegate<Args...>>(this, [](FTAMulticastDelegate<Args...>* obj){});
    }

    template <typename Obj>
    HndlPtr AddFunc(Obj* Object, void (Obj::*Function)(Args...))
    {
        std::shared_ptr<FCallable> Callable = std::make_shared<FCallable>();
        struct FFuncStorage
        {
            void (Obj::*FuncPtr)(Args...) = nullptr;
        };
        FFuncStorage* Storage = new FFuncStorage();
        Storage->FuncPtr = Function;
        Callable->Func_Obj = Storage;
        Callable->Deleter = [](void* Func_Obj){delete static_cast<FFuncStorage*>(Func_Obj);};
        Callable->Obj_void = Object;

        Callable->Func = [](void* Func_Obj, void* Obj_void, Args... args)
        {
            (static_cast<Obj*>(Obj_void)->*(static_cast<FFuncStorage*>(Func_Obj)->FuncPtr))(args...);
        };
        int NewInd = ++Index;
        Callbacks.emplace(NewInd, Callable);
        return std::make_shared<Hndl>(SelfShared, NewInd);
    }
    
    void Remove(int Handle) {
        Callbacks.erase(Handle);
    }

    void Invoke(Args... args) {
        auto CallbacksTemp = Callbacks;
        for (auto& Callback : CallbacksTemp) {
            Callback.second->Call(args...);
        }
    }

private:
    std::map<int, std::shared_ptr<FCallable>> Callbacks;
    std::shared_ptr<FTAMulticastDelegate> SelfShared;
    static inline int Index = 0;
};

