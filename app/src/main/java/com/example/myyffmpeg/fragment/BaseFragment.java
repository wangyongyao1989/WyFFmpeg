package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;


public abstract class BaseFragment extends Fragment {

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        View layoutDataBing = getLayoutDataBing(inflater, container, savedInstanceState);
        initView();
        initData();
        initObserver();
        initListener();
        return layoutDataBing;
    }

    public abstract View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState);

    public abstract void initView();

    public abstract void initData();

    public abstract void initObserver();

    public abstract void initListener();

}
